//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.06
//    Bug Fix:  Set GPP_C8 to GPO Low.
//    Reason:   
//    Auditor:  William Hsu
//    Date:     Jan/12/2017
//
//
//  Rev. 1.05
//    Bug Fix:  Set GPP_H20 to input pin for sSATA/PCIe combo port 2 dynamic detect by GPIO.
//    Reason:   
//    Auditor:  Jimmy Chiu
//    Date:     Oct/21/2016
//
//  Rev. 1.04
//    Bug Fix:  Modify GPP_C9 attritue to fix swjpme2 can't work issue.
//    Reason:   GpioResetPwrGood : maps to RSMRST for keep GPIO attribute during CF9 reset.
//    Auditor:  Jimmy Chiu
//    Date:     Sep/26/2016
//
//  Rev. 1.03
//    Bug Fix:  Set GPP_C22 as BMC SMI.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/18/2016
//
//  Rev. 1.02
//    Bug Fix:  Disable clock req pins.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/15/2016
//
//  Rev. 1.01
//    Bug Fix:  Set GPP_D0 as NMI attribute.
//    Reason:   For BMC_NMI.
//    Auditor:  Kasber Chen
//    Date:     Jun/24/2016
//
//  Rev. 1.00
//    Bug Fix:  Set GPP_D3 as NMI attribute.
//    Reason:   For SW_NMI.
//    Auditor:  Jimmy Chiu (Refer Greenlow)
//    Date:     Jun/13/2016
//
//****************************************************************************
/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2013-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  GpioTable.c

Abstract:

--*/


#include "UbaMainPei.h"
#include <Library/UbaGpioUpdateLib.h>

#include <Library/GpioLib.h>
#include <Library/UbaGpioInitLib.h>
#include <GpioPinsSklH.h>
#include <Library/PcdLib.h>

static GPIO_INIT_CONFIG mGpioTableBarkPeakEP [] =
{
    {GPIO_SKL_H_GPP_A0,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// LPC_RCIN_N_ESPI_ALERT1_N [GPP_A_0_RCINB_ESPI_ALERT1B]
    {GPIO_SKL_H_GPP_A1,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// LPC_LAD0_ESPI_IO0 [GPP_A_1_LAD_0_ESPI_IO_0]
    {GPIO_SKL_H_GPP_A2,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// LPC_LAD1_ESPI_IO1 [GPP_A_2_LAD_1_ESPI_IO_1]
    {GPIO_SKL_H_GPP_A3,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// LPC_LAD2_ESPI_IO2 [GPP_A_3_LAD_2_ESPI_IO_2]
    {GPIO_SKL_H_GPP_A4,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// LPC_LAD3_ESPI_IO3 [GPP_A_4_LAD_3_ESPI_IO_3]
    {GPIO_SKL_H_GPP_A5,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// LPC_LFRAME_N_ESPI_CS0_N [GPP_A_5_LFRAMEB_ESPI_CS0B]
    {GPIO_SKL_H_GPP_A6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// IRQ_LPC_SERIRQ_ESPI_CS1_N [GPP_A_6_SERIRQ_ESPI_CS1B]
    {GPIO_SKL_H_GPP_A7,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// IRQ_LPC_PIRQA_N_ESPI_ALERT0_N [GPP_A_7_PIRQAB_ESPI_ALERT0B]
    {GPIO_SKL_H_GPP_A8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// FM_LPC_CLKRUN_N [GPP_A_8_CLKRUNB]
    {GPIO_SKL_H_GPP_A9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// CLK_24M_66M_LPC0_ESPI [GPP_A_9_CLKOUT_LPC_0_ESPI_CLK]
    {GPIO_SKL_H_GPP_A10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// TP_PCH_GPP_A_10 [GPP_A_10_CLKOUT_LPC_1]
    {GPIO_SKL_H_GPP_A11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// FM_LPC_PME_N [GPP_A_11_PMEB]
    {GPIO_SKL_H_GPP_A12, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv,    GpioOutLow,     GpioIntLevel | GpioIntSci,GpioResetNormal, GpioTermNone}},//IRQ_PCH_SCI_WHEA_N [GPP_A_12_BM_BUSYB_SXEXITHLDOFFB]
    {GPIO_SKL_H_GPP_A13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// FM_EUP_LOT6_N [GPP_A_13_SUSWARNB_SUSPWRDNACK]
    {GPIO_SKL_H_GPP_A14, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// RST_ESPI_RESET_N [GPP_A_14_ESPI_RESETB]
    {GPIO_SKL_H_GPP_A15, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// FM_SUSACK_N [GPP_A_15_SUSACKB]
    {GPIO_SKL_H_GPP_A16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// CLK_48MHZ_PCH [GPP_A_16_CLKOUT_LPC_2]
    {GPIO_SKL_H_GPP_A17, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},// FM_KTI_SLOW_MODE_N [GPP_A_17]
    {GPIO_SKL_H_GPP_A18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},// FM_BIOS_ADV_FUNCTIONS [GPP_A_18]
    {GPIO_SKL_H_GPP_A19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},// FM_ME_RCVR_N [GPP_A_19]
    {GPIO_SKL_H_GPP_A20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},// TP_PCH_GPP_A_20 [GPP_A_20]
    {GPIO_SKL_H_GPP_A21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},// TP_PCH_GPP_A_21 [GPP_A_21]
    {GPIO_SKL_H_GPP_A22, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},// TP_PCH_GPP_A_22 [GPP_A_22]
    {GPIO_SKL_H_GPP_A23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},// TP_PCH_GPP_A_23 [GPP_A_23]
    {GPIO_SKL_H_GPP_B0,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_PCH_CORE_VID_0 [GPP_B_0_CORE_VID_0]
    {GPIO_SKL_H_GPP_B1,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_PCH_CORE_VID_1 [GPP_B_1_CORE_VID_1]
    {GPIO_SKL_H_GPP_B2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//PU_PCH_VRALERT_N [GPP_B_2_VRALERTB]
    {GPIO_SKL_H_GPP_B3,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_QAT_ENABLE_N [GPP_B_3_CPU_GP_2]
    {GPIO_SKL_H_GPP_B4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_B_4 [GPP_B_4_CPU_GP_3]
    {GPIO_SKL_H_GPP_B5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//PU_PCH_GPP_B_5 [GPP_B_5_SRCCLKREQB_0]
    {GPIO_SKL_H_GPP_B6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//PU_PCH_GPP_B_6 [GPP_B_6_SRCCLKREQB_1]
    {GPIO_SKL_H_GPP_B7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//PU_PCH_GPP_B_7 [GPP_B_7_SRCCLKREQB_2]
    {GPIO_SKL_H_GPP_B8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//PU_PCH_GPP_B_8 [GPP_B_8_SRCCLKREQB_3]
    {GPIO_SKL_H_GPP_B9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//PU_PCH_GPP_B_9 [GPP_B_9_SRCCLKREQB_4]
    {GPIO_SKL_H_GPP_B10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//PU_PCH_GPP_B_10 [GPP_B_10_SRCCLKREQB_5]
    {GPIO_SKL_H_GPP_B11, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_PMBUS_ALERT_B_EN [GPP_B_11]
    {GPIO_SKL_H_GPP_B12, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//TP_SLP_S0_N [GPP_B_12_SLP_S0B]
    {GPIO_SKL_H_GPP_B13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//RST_PLTRST_N [GPP_B_13_PLTRSTB]
    {GPIO_SKL_H_GPP_B14, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone}},//FM_PCH_BIOS_RCVR_SPKR [GPP_B_14_SPKR]
    {GPIO_SKL_H_GPP_B15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_CPU_ERR0_LVT3_N [GPP_B_15]
    {GPIO_SKL_H_GPP_B16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_CPU_ERR1_LVT3_N [GPP_B_16]
    {GPIO_SKL_H_GPP_B17, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_B_17 [GPP_B_17]
    {GPIO_SKL_H_GPP_B18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//PU_NO_REBOOT [GPP_B_18]
    {GPIO_SKL_H_GPP_B19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_B_19 [GPP_B_19]
    {GPIO_SKL_H_GPP_B20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//FM_BIOS_POST_CMPLT_N [GPP_B_20]
    {GPIO_SKL_H_GPP_B21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_LINK_WIDTH_ID5 [GPP_B_21]
    {GPIO_SKL_H_GPP_B22, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_PCH_BOOT_BIOS_DEVICE [GPP_B_22]
    {GPIO_SKL_H_GPP_B23, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_PCH_BMC_THERMTRIP_N [GPP_B_23MEIE_SML1ALERTB_PCHHOTB]
    {GPIO_SKL_H_GPP_C5,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//IRQ_SML0_ALERT_N [GPP_C_5_SML0ALERTB]
    {GPIO_SKL_H_GPP_C8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutLow,     GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//FM_PASSWORD_CLEAR_N [GPP_C_8_UART0_RXD]
    {GPIO_SKL_H_GPP_C9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//FM_MFG_MODE [GPP_C_9_UART0_TXD]
    {GPIO_SKL_H_GPP_C10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInOut, GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone}},//FM_PCH_SATA_RAID_KEY [GPP_C_10_UART0_RTSB]
    {GPIO_SKL_H_GPP_C11, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutLow,     GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//TP_FP_AUD_DETECT_N [GPP_C_11_UART0_CTSB]
    {GPIO_SKL_H_GPP_C12, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_BOARD_REV_ID0 [GPP_C_12_UART1_RXD_ISH_UART1_RXD]
    {GPIO_SKL_H_GPP_C13, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_BOARD_REV_ID1 [GPP_C_13_UART1_TXD_ISH_UART1_TXD]
    {GPIO_SKL_H_GPP_C14, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutLow,     GpioIntLevel | GpioIntSci,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//FM_BMC_PCH_SCI_LPC_N [GPP_C_14_UART1_RTSB_ISH_UART1_RTSB]
    {GPIO_SKL_H_GPP_C15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//TP_LINK_WIDTH_ID0 [GPP_C_15_UART1_CTSB_ISH_UART1_CTSB]
    {GPIO_SKL_H_GPP_C16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//TP_LINK_WIDTH_ID1 [GPP_C_16_I2C0_SDA]
    {GPIO_SKL_H_GPP_C17, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//TP_LINK_WIDTH_ID2 [GPP_C_17_I2C0_SCL]
    {GPIO_SKL_H_GPP_C18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//TP_LINK_WIDTH_ID3 [GPP_C_18_I2C1_SDA]
    {GPIO_SKL_H_GPP_C19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//TP_LINK_WIDTH_ID4 [GPP_C_19_I2C1_SCL]
    {GPIO_SKL_H_GPP_C20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_THROTTLE_N [GPP_C_20_UART2_RXD]
    {GPIO_SKL_H_GPP_C21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//RST_PCH_MIC_MUX_N [GPP_C_21_UART2_TXD]
    {GPIO_SKL_H_GPP_C22, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutLow,     GpioIntLevel | GpioIntSmi,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//IRQ_BMC_PCH_SMI_LPC_N [GPP_C_22_UART2_RTSB]
    {GPIO_SKL_H_GPP_C23, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirIn,    GpioOutLow,     GpioIntLevel | GpioIntSmi,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//FM_CPU_CATERR_DLY_LVT3_N [GPP_C_23_UART2_CTSB]
    {GPIO_SKL_H_GPP_D0,  { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirIn,    GpioOutHigh, GpioIntLevel | GpioIntNmi,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//IRQ_BMC_PCH_NMI [GPP_D_0_SPI1_CSB]
    {GPIO_SKL_H_GPP_D1,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,  GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FP_PWR_LED_N [GPP_D_1_SPI1_CLK]
    {GPIO_SKL_H_GPP_D2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,  GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_TBT_FORCE_PWR [GPP_D_2_SPI1_MISO_IO_1]
    {GPIO_SKL_H_GPP_D3,  { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirIn,    GpioOutHigh, GpioIntLevel | GpioIntSci,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//FM_TBT_SCI_EVENT [GPP_D_3_SPI1_MOSI_IO_0]
    {GPIO_SKL_H_GPP_D4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,  GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_PLD_PCH_DATA [GPP_D_4_ISH_I2C2_SDA_I2C3_SDA]
    {GPIO_SKL_H_GPP_D5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_D_5 [GPP_D_5_SSP0_SFRM]
    {GPIO_SKL_H_GPP_D6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_D_6 [GPP_D_6_SSP0_TXD]
    {GPIO_SKL_H_GPP_D7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_D_7 [GPP_D_7_SSP0_RXD]
    {GPIO_SKL_H_GPP_D8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_D_8 [GPP_D_8_SSP0_SCLK]
    {GPIO_SKL_H_GPP_D9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_D_9 [GPP_D_9_ISH_SPI_CSB]
    {GPIO_SKL_H_GPP_D10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_M2_SDD_DEVSLP [GPP_D_10_ISH_SPI_CLK]
    {GPIO_SKL_H_GPP_D11, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_LA_TRIGGER_N [GPP_D_11_ISH_SPI_MISO]
    {GPIO_SKL_H_GPP_D12, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//SGPIO_SSATA_DATA1 [GPP_D_12_ISH_SPI_MOSI]
    {GPIO_SKL_H_GPP_D13, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInOut, GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//SMB_SMLINK5_STBY_LVC3_SDA [GPP_D_13_ISH_UART0_RXD_SML0BDATA_I2C2_SDA]
    {GPIO_SKL_H_GPP_D14, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInOut, GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//SMB_SMLINK5_STBY_LVC3_SCL [GPP_D_14_ISH_UART0_TXD_SML0BCLK_I2C2_SCL]
    {GPIO_SKL_H_GPP_D15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//SGPIO_SSATA_DATA0 [GPP_D_15_ISH_UART0_RTSB]
    {GPIO_SKL_H_GPP_D16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_D_16 [GPP_D_16_ISH_UART0_CTSB_SML0BALERTB]
    {GPIO_SKL_H_GPP_D17, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_D_17 [GPP_D_17_DMIC_CLK_1]
    {GPIO_SKL_H_GPP_D18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_D_18 [GPP_D_18_DMIC_DATA_1]
    {GPIO_SKL_H_GPP_D19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_PS_PWROK_DLY_SEL [GPP_D_19_DMIC_CLK_0]
    {GPIO_SKL_H_GPP_D20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_D_20 [GPP_D_20_DMIC_DATA_0]
    {GPIO_SKL_H_GPP_D21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SPC_IE_LVC3_RX [GPP_D_21_SPI1_IO_2]
    {GPIO_SKL_H_GPP_D22, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//SPC_IE_LVC3_TX [GPP_D_22_SPI1_IO_3]
    {GPIO_SKL_H_GPP_D23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_D_23 [GPP_D_23_ISH_I2C2_SCL_I2C3_SCL]
    {GPIO_SKL_H_GPP_E0,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_E_0 [GPP_E_0_SATAXPCIE_0_SATAGP_0]
    {GPIO_SKL_H_GPP_E1,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_E_1 [GPP_E_1_SATAXPCIE_1_SATAGP_1]
    {GPIO_SKL_H_GPP_E2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_E_2 [GPP_E_2_SATAXPCIE_2_SATAGP_2]
    {GPIO_SKL_H_GPP_E3,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutLow, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_ADR_TRIGGER_N [GPP_E_3_CPU_GP0]
    {GPIO_SKL_H_GPP_E4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_E_4 [GPP_E_4_SATA_DEVSLP_0]
    {GPIO_SKL_H_GPP_E5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_E_5 [GPP_E_5_SATA_DEVSLP_1]
    {GPIO_SKL_H_GPP_E6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone}},//TP_PCH_GPP_E_6 [GPP_E_6_SATA_DEVSLP_2]
    {GPIO_SKL_H_GPP_E7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_ADR_SMI_GPIO_N [GPP_E_7_CPU_GP1]
    {GPIO_SKL_H_GPP_E8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//LED_PCH_SATA_HDD_N [GPP_E_8_SATA_LEDB]
    {GPIO_SKL_H_GPP_E9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_OC0_USB_N [GPP_E_9_USB2_OCB_0]
    {GPIO_SKL_H_GPP_E10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_OC1_USB_N [GPP_E_10_USB2_OCB_1]
    {GPIO_SKL_H_GPP_E11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_OC2_USB_N [GPP_E_11_USB2_OCB_2]
    {GPIO_SKL_H_GPP_E12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_OC3_USB_N [GPP_E_12_USB2_OCB_3]
    {GPIO_SKL_H_GPP_F0,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_F_0 [GPP_F_0_SATAXPCIE_3_SATAGP_3]
    {GPIO_SKL_H_GPP_F1,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_F_1 [GPP_F_1_SATAXPCIE_4_SATAGP_4]
    {GPIO_SKL_H_GPP_F2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_F_2 [GPP_F_2_SATAXPCIE_5_SATAGP_5]
    {GPIO_SKL_H_GPP_F3,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_F_3 [GPP_F_3_SATAXPCIE_6_SATAGP_6]
    {GPIO_SKL_H_GPP_F4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_F_4 [GPP_F_4_SATAXPCIE_7_SATAGP_7]
    {GPIO_SKL_H_GPP_F5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutLow, GpioIntLevel | GpioIntApic,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//IRQ_TPM_SPI _N [GPP_F_5_SATA_DEVSLP_3]
    {GPIO_SKL_H_GPP_F6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//JTAG_PCH_PLD_TCK [GPP_F_6_SATA_DEVSLP_4]
    {GPIO_SKL_H_GPP_F7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//JTAG_PCH_PLD_TDI [GPP_F_7_SATA_DEVSLP_5]
    {GPIO_SKL_H_GPP_F8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//JTAG_PCH_PLD_TMS [GPP_F_8_SATA_DEVSLP_6]
    {GPIO_SKL_H_GPP_F9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//JTAG_PCH_PLD_TDO [GPP_F_9_SATA_DEVSLP_7]
    {GPIO_SKL_H_GPP_F10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SGPIO_SATA_CLOCK [GPP_F_10_SATA_SCLOCK]
    {GPIO_SKL_H_GPP_F11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SGPIO_SATA_LOAD [GPP_F_11_SATA_SLOAD]
    {GPIO_SKL_H_GPP_F12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SGPIO_SATA_DATA1 [GPP_F_12_SATA_SDATAOUT1]
    {GPIO_SKL_H_GPP_F13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SGPIO_SATA_DATA0 [GPP_F_13_SATA_SDATAOUT0]
    {GPIO_SKL_H_GPP_F14, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_14_SSATA_LEDB
    {GPIO_SKL_H_GPP_F15, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_OC4_USB_N [GPP_F_15_USB2_OCB_4]
    {GPIO_SKL_H_GPP_F16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_OC5_USB_N [GPP_F_16_USB2_OCB_5]
    {GPIO_SKL_H_GPP_F17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_OC6_USB_N [GPP_F_17_USB2_OCB_6]
    {GPIO_SKL_H_GPP_F18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_OC7_USB_N [GPP_F_18_USB2_OCB_7]
    {GPIO_SKL_H_GPP_F19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//SMB_SMLINK5_STBY_LVC3_SCL [GPP_F_19_L_VDDEN]
    {GPIO_SKL_H_GPP_F20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInOut, GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//SMB_SMLINK5_STBY_LVC3_SDA [GPP_F_20_L_BKLTEN]
    {GPIO_SKL_H_GPP_F21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_F_21 [GPP_F_21_L_BKLTCTL]
    {GPIO_SKL_H_GPP_F22, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SGPIO_SSATA_CLOCK [GPP_F_22]
    {GPIO_SKL_H_GPP_F23, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SGPIO_SSATA_LOAD [GPP_F_23]
    {GPIO_SKL_H_GPP_G0,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FAN_PCH_TACH_0 [GPP_G_0_FAN_TACH_0]
    {GPIO_SKL_H_GPP_G1,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FAN_PCH_TACH_1 [GPP_G_1_FAN_TACH_1]
    {GPIO_SKL_H_GPP_G2,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FAN_PCH_TACH_2 [GPP_G_2_FAN_TACH_2]
    {GPIO_SKL_H_GPP_G3,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FAN_PCH_TACH_3 [GPP_G_3_FAN_TACH_3]
    {GPIO_SKL_H_GPP_G4,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FAN_PCH_TACH_4 [GPP_G_4_FAN_TACH_4]
    {GPIO_SKL_H_GPP_G5,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FAN_PCH_TACH_5 [GPP_G_5_FAN_TACH_5]
    {GPIO_SKL_H_GPP_G6,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FAN_PCH_TACH_6 [GPP_G_6_FAN_TACH_6]
    {GPIO_SKL_H_GPP_G7,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FAN_PCH_TACH_7 [GPP_G_7_FAN_TACH_7]
    {GPIO_SKL_H_GPP_G8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FAN_PCH_PWM_0 [GPP_G_8_FAN_PWM_0]
    {GPIO_SKL_H_GPP_G9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FAN_PCH_PWM_1 [GPP_G_9_FAN_PWM_1]
    {GPIO_SKL_H_GPP_G10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FAN_PCH_PWM_2 [GPP_G_10_FAN_PWM_2]
    {GPIO_SKL_H_GPP_G11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FAN_PCH_PWM_3 [GPP_G_11_FAN_PWM_3]
    {GPIO_SKL_H_GPP_G12, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//FM_BOARD_SKU_ID0 [GPP_G_12_GSXDOUT]
    {GPIO_SKL_H_GPP_G13, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//FM_BOARD_SKU_ID1 [GPP_G_13_GSXSLOAD]
    {GPIO_SKL_H_GPP_G14, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//FM_BOARD_SKU_ID2 [GPP_G_14_GSXDIN]
    {GPIO_SKL_H_GPP_G15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//FM_BOARD_SKU_ID3 [GPP_G_15_GSXSRESETB]
    {GPIO_SKL_H_GPP_G16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//FM_BOARD_SKU_ID4 [GPP_G_16_GSXCLK]
    {GPIO_SKL_H_GPP_G17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_ADR_COMPLETE [GPP_G_17_ADR_COMPLETE]
    {GPIO_SKL_H_GPP_G18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_NMI_EVENT_N [GPP_G_18_NMIB]
    {GPIO_SKL_H_GPP_G19, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_SMI_ACTIVE_N [GPP_G_19_SMIB]
    {GPIO_SKL_H_GPP_G21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_SATAEXPRESS_DEVSLP [GPP_G_21]
    {GPIO_SKL_H_GPP_G23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_SSATA_PCIE_SEL [GPP_G_23]
    {GPIO_SKL_H_GPP_H0,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//PU_PCH_GPP_H_0 [GPP_H_0_SRCCLKREQB_6]
    {GPIO_SKL_H_GPP_H2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInOut, GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_PCH_MGPIO_TEST0 [GPP_H_2_SRCCLKREQB_8]
    {GPIO_SKL_H_GPP_H3,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInOut, GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_PCH_MGPIO_TEST1 [GPP_H_3_SRCCLKREQB_9]
    {GPIO_SKL_H_GPP_H4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInOut, GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_PCH_MGPIO_TEST4 [GPP_H_4_SRCCLKREQB_10]
    {GPIO_SKL_H_GPP_H6,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_OCULINK1_PCIE_SSD0_PRSNT_N [GPP_H_6_SRCCLKREQB_12]
    {GPIO_SKL_H_GPP_H7,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_OCULINK1_PCIE_SSD1_PRSNT_N [GPP_H_7_SRCCLKREQB_13]
    {GPIO_SKL_H_GPP_H8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_CLKREQ_NIC1_N [GPP_H_8_SRCCLKREQB_14]
    {GPIO_SKL_H_GPP_H9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInOut, GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_PCH_MGPIO_TEST5 [GPP_H_9_SRCCLKREQB_15]
    {GPIO_SKL_H_GPP_H10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMB_SMLINK2_STBY_LVC3_SCL [GPP_H_10_SML2CLK]
    {GPIO_SKL_H_GPP_H11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMB_SMLINK2_STBY_LVC3_SDA [GPP_H_11_SML2DATA]
    {GPIO_SKL_H_GPP_H12, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_ESPI_FLASH_MODE [GPP_H_12_SML2ALERTB]
    {GPIO_SKL_H_GPP_H13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMB_SMLINK3_STBY_LVC3_SCL [GPP_H_13_SML3CLK]
    {GPIO_SKL_H_GPP_H14, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMB_SMLINK3_STBY_LVC3_SDA [GPP_H_14_SML3DATA]
    {GPIO_SKL_H_GPP_H15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutLow, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//PU_ADR_TIMER_HOLD_OFF_N [GPP_H_15_SML3ALERTB]
    {GPIO_SKL_H_GPP_H16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMB_SMLINK4_STBY_LVC3_SCL [GPP_H_16_SML4CLK]
    {GPIO_SKL_H_GPP_H17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMB_SMLINK4_STBY_LVC3_SDA [GPP_H_17_SML4DATA]
    {GPIO_SKL_H_GPP_H18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutLow, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//FM_LT_KEY_DOWNGRADE_N [GPP_H_18_SML4ALERTB]
    {GPIO_SKL_H_GPP_H19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_H_19 [GPP_H_19]
    {GPIO_SKL_H_GPP_H20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInOut, GpioOutHigh, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_PCH_MGPIO_TEST2 [GPP_H_20]
    {GPIO_SKL_H_GPP_H21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInOut, GpioOutHigh, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_PCH_MGPIO_TEST3 [GPP_H_21]
    {GPIO_SKL_H_GPP_H22, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_H_22 [GPP_H_22]
    {GPIO_SKL_H_GPP_H23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//FM_SSATA_PCIE_M2_SEL [GPP_H_23]
    {GPIO_SKL_H_GPP_I0,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,  GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//JTAG_GBE_TDO [GPP_I_0_DDSP_HPD_0]
    {GPIO_SKL_H_GPP_I1,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//JTAG_GBE_TCK [GPP_I_1_DDSP_HPD_1]
    {GPIO_SKL_H_GPP_I2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//JTAG_GBE_TMS [GPP_I_2_DDSP_HPD_2]
    {GPIO_SKL_H_GPP_I3,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//JTAG_GBE_TDI [GPP_I_3_DDSP_HPD_3]
    {GPIO_SKL_H_GPP_I4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FP_LED_STATUS_GREEN_PCH_N [GPP_I_4_EDP_HPD]
    {GPIO_SKL_H_GPP_I5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FP_LED_STATUS_AMBER_PCH_N [GPP_I_5_DDPB_CTRLCLK]
    {GPIO_SKL_H_GPP_I6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FP_ID_LED_PCH_N [GPP_I_6_DDPB_CTRLDATA]
    {GPIO_SKL_H_GPP_I7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutLow, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//JTAG_GBE_TRST_N [GPP_I_7_DDPC_CTRLCLK]
    {GPIO_SKL_H_GPP_I8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutLow, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FP_ID_BTN_PCH_N [GPP_I_8_DDPC_CTRLDATA]
    {GPIO_SKL_H_GPP_I9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutLow, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//FM_MEM_THERM_EVENT_PCH_N [GPP_I_9_DDPD_CTRLCLK]
    {GPIO_SKL_H_GPP_I10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_I_10 [GPP_I_10_DDPD_CTRLDATA]
    {GPIO_SKL_H_GPD0,    { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//FM_FIVRBREAK_N [GPD_0_BATLOWB]
    {GPIO_SKL_H_GPD1,    { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,  GpioOutHigh, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//PU_ACPRESENT [GPD_1_ACPRESENT]
    {GPIO_SKL_H_GPD2,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//FM_LAN_WAKE_N [GPD_2_LAN_WAKEB]
    {GPIO_SKL_H_GPD3,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//FM_PCH_PWRBTN_N [GPD_3_PWRBTNB]
    {GPIO_SKL_H_GPD4,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//FM_SLPS3_N [GPD_4_SLP_S3B]
    {GPIO_SKL_H_GPD5,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//FM_SLPS4_N [GPD_5_SLP_S4B]
    {GPIO_SKL_H_GPD6,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//FM_SLPA_N [GPD_6_SLP_AB]
    {GPIO_SKL_H_GPD7,    { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//TP_GPD_7 [GPD_7_USB2_WAKEOUTB]
    {GPIO_SKL_H_GPD8,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//CLK_33K_PCH_SUSCLK_PLD [GPD_8_SUSCLK]
    {GPIO_SKL_H_GPD9,    { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//TP_SLP_WLAN [GPD_9_SLP_WLANB]
    {GPIO_SKL_H_GPD10,   { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//FM_SLPS5_N [GPD_10_SLP_S5B]
    {GPIO_SKL_H_GPD11,   { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//FM_PHY_DISABLE_N [GPD_11_LANPHYPC]
};

static GPIO_INIT_CONFIG mGpioTableNeonCityEP [] =
  {
    {GPIO_SKL_H_GPP_A0,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_0_LPC_RCIN_N_ESPI_ALERT1_N
    {GPIO_SKL_H_GPP_A1,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_1_LAD_0_ESPI_IO_0
    {GPIO_SKL_H_GPP_A2,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_2_LAD_1_ESPI_IO_1
    {GPIO_SKL_H_GPP_A3,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_3_LAD_2_ESPI_IO_2
    {GPIO_SKL_H_GPP_A4,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_4_LAD_3_ESPI_IO_3
    {GPIO_SKL_H_GPP_A5,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_5_LPC_LFRAME_N_ESPI_CS0_N
    {GPIO_SKL_H_GPP_A6,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_6_IRQ_LPC_SERIRQ_ESPI_CS1_N
    {GPIO_SKL_H_GPP_A7,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_7_IRQ_LPC_PIRQA_N_ESPI_ALERT0_N
    {GPIO_SKL_H_GPP_A8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_8_FM_LPC_CLKRUN_N
    {GPIO_SKL_H_GPP_A9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_9_CLK_24M_66M_LPC0_ESPI
    {GPIO_SKL_H_GPP_A10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_10_TP_PCH_GPP_A_10
    {GPIO_SKL_H_GPP_A11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_11_FM_LPC_PME_N
    {GPIO_SKL_H_GPP_A12, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci, GpioResetNormal,  GpioTermNone}},//GPP_A_12_IRQ_PCH_SCI_WHEA_N
    {GPIO_SKL_H_GPP_A13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_13_FM_EUP_LOT6_N
    {GPIO_SKL_H_GPP_A14, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_14_ESPI_RESET_N
    {GPIO_SKL_H_GPP_A15, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_15_SUSACK_N
    {GPIO_SKL_H_GPP_A16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_16_TP_PCH_GPP_A_16
    {GPIO_SKL_H_GPP_A17, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_17_FM_KTI_SLOW_MODE_N
    {GPIO_SKL_H_GPP_A18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_A_18_FM_BIOS_ADV_FUNCTIONS
    {GPIO_SKL_H_GPP_A20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_20_TP_PCH_GPP_A_20
    {GPIO_SKL_H_GPP_A21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_21_TP_PCH_GPP_A_21
    {GPIO_SKL_H_GPP_A22, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_22_TP_PCH_GPP_A_22
    {GPIO_SKL_H_GPP_A23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_23_TP_PCH_GPP_A_23
    {GPIO_SKL_H_GPP_B0,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_0_CORE_VID_0
    {GPIO_SKL_H_GPP_B1,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_1_CORE_VID_1
    {GPIO_SKL_H_GPP_B2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_2_VRALERT_N
    {GPIO_SKL_H_GPP_B3,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_3_FM_QAT_ENABLE_N
    {GPIO_SKL_H_GPP_B4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_4_TP_PCH_GPP_B_4
    {GPIO_SKL_H_GPP_B5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_5_PU_PCH_GPP_B_5
    {GPIO_SKL_H_GPP_B6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_B_6_PU_PCH_GPP_B_6
    {GPIO_SKL_H_GPP_B7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_B_7_PU_PCH_GPP_B_7
    {GPIO_SKL_H_GPP_B8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_8_PU_PCH_GPP_B_8
    {GPIO_SKL_H_GPP_B9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_9_PU_PCH_GPP_B_9
    {GPIO_SKL_H_GPP_B10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_10_PU_PCH_GPP_B_10
    {GPIO_SKL_H_GPP_B12, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_B_12_GLB_RST_WARN_N
    {GPIO_SKL_H_GPP_B13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_13_RST_PLTRST_N
    {GPIO_SKL_H_GPP_B14, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone}},//GPP_B_14_FM_PCH_BIOS_RCVR_SPKR
    {GPIO_SKL_H_GPP_B15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_15_FM_CPU_ERR0_LVT3_N
    {GPIO_SKL_H_GPP_B16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_16_FM_CPU_ERR1_LVT3_N
    {GPIO_SKL_H_GPP_B17, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_17_TP_PCH_GPP_B_17
    {GPIO_SKL_H_GPP_B18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_18_PU_NO_REBOOT
    {GPIO_SKL_H_GPP_B19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_19_TP_PCH_GPP_B_19
    {GPIO_SKL_H_GPP_B20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_B_20_FM_BIOS_POST_CMPLT_N
    {GPIO_SKL_H_GPP_B21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_21_TP_LINK_WIDTH_ID5
    {GPIO_SKL_H_GPP_B22, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_22_FM_PCH_BOOT_BIOS_DEVICE
    {GPIO_SKL_H_GPP_B23, { GpioPadModeNative2, GpioHostOwnGpio, GpioDirInOut, GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_23_FM_PCH_BMC_THERMTRIP_EXI_STRAP_N
    {GPIO_SKL_H_GPP_C5,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_C_5_IRQ_SML0_ALERT_N
    {GPIO_SKL_H_GPP_C8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_C_8_10G_LAN_DIS X11SPM X11SPH
    {GPIO_SKL_H_GPP_C9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetPwrGood,GpioTermNone, GpioPadConfigLock}},//SWJPME2 SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_C10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInOut, GpioOutHigh,    GpioIntDis,GpioResetNormal,GpioTermNone}},//GPP_C_10_FM_PCH_SATA_RAID_KEY
    {GPIO_SKL_H_GPP_C11, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_C_11_TP_FP_AUD_DETECT_N
    {GPIO_SKL_H_GPP_C12, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_C_12_FM_BOARD_REV_ID0
    {GPIO_SKL_H_GPP_C13, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_C_13_FM_BOARD_REV_ID1
    {GPIO_SKL_H_GPP_C14, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_C_14_FM_BMC_PCH_SCI_LPC_N
    {GPIO_SKL_H_GPP_C15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_C_15_TP_LINK_WIDTH_ID0
    {GPIO_SKL_H_GPP_C16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_C_16_TP_LINK_WIDTH_ID1
    {GPIO_SKL_H_GPP_C17, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_C_17_TP_LINK_WIDTH_ID2
    {GPIO_SKL_H_GPP_C18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_C_18_TP_LINK_WIDTH_ID3
    {GPIO_SKL_H_GPP_C19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_C_19_TP_LINK_WIDTH_ID4
    {GPIO_SKL_H_GPP_C21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_C_21_RST_PCH_MIC_MUX_N
    {GPIO_SKL_H_GPP_C22, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntEdge | GpioIntSmi,GpioResetNormal,  GpioTermWpu20K, GpioPadConfigLock}},//GPP_C_22_IRQ_BMC_PCH_SMI_LPC_N //SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_C23, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntSmi,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_C_23_FM_CPU_CATERR_DLY_LVT3_N
    {GPIO_SKL_H_GPP_D0,  { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntEdge | GpioIntNmi,GpioResetNormal,  GpioTermWpu20K, GpioPadConfigLock}},//BMC_NMI //SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_D1,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_D_1_FP_PWR_LED_N
//  {GPIO_SKL_H_GPP_D2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_D_2_FM_TBT_FORCE_PWR
    {GPIO_SKL_H_GPP_D2,  { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntEdge | GpioIntNmi,GpioResetNormal,  GpioTermWpu20K, GpioPadConfigLock}},//CPLD NMI//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_D3,  { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntEdge | GpioIntNmi,GpioResetNormal,  GpioTermWpu20K, GpioPadConfigLock}},//External_NMI //SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_D4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_D_4_FM_PLD_PCH_DATA
    {GPIO_SKL_H_GPP_D5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_5_TP_PCH_GPP_D_5
    {GPIO_SKL_H_GPP_D6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_6_TP_PCH_GPP_D_6
    {GPIO_SKL_H_GPP_D7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_7_TP_PCH_GPP_D_7
    {GPIO_SKL_H_GPP_D8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_8_TP_PCH_GPP_D_8
    {GPIO_SKL_H_GPP_D9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_9_TP_PCH_GPP_D_9
    {GPIO_SKL_H_GPP_D10, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_10_FM_M2_SSD_DEVSLP
    {GPIO_SKL_H_GPP_D11, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_D_11_FM_LA_TRIGGER_N
    {GPIO_SKL_H_GPP_D12, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_12_SGPIO_SSATA_DATA1
    {GPIO_SKL_H_GPP_D13, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_13_SMB_SMLINK5_STBY_LVC3_SCL
    {GPIO_SKL_H_GPP_D14, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_14_SMB_SMLINK5_STBY_LVC3_SDA
    {GPIO_SKL_H_GPP_D15, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_15_SSATA_SDATAOUT0
    {GPIO_SKL_H_GPP_D16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_16_TP_PCH_GPP_D_16
    {GPIO_SKL_H_GPP_D17, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_17_TP_PCH_GPP_D_17
    {GPIO_SKL_H_GPP_D18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_D_18_TP_PCH_GPP_D_18
    {GPIO_SKL_H_GPP_D19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_D_19_FM_PS_PWROK_DLY_SEL
    {GPIO_SKL_H_GPP_D20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_20_TP_PCH_GPP_D_20
    {GPIO_SKL_H_GPP_D21, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_21_SPC_IE_LVC3_RX
    {GPIO_SKL_H_GPP_D22, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_22_SPC_IE_LVC3_TX
    {GPIO_SKL_H_GPP_D23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_23_TP_PCH_GPP_D_23
    {GPIO_SKL_H_GPP_E0,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_0_TP_PCH_GPP_E_0
    {GPIO_SKL_H_GPP_E1,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_1_TP_PCH_GPP_E_1
    {GPIO_SKL_H_GPP_E2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_2_TP_PCH_GPP_E_2
    {GPIO_SKL_H_GPP_E3,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_3_FM_ADR_TRIGGER_N
    {GPIO_SKL_H_GPP_E4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_4_TP_PCH_GPP_E_4
    {GPIO_SKL_H_GPP_E5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_5_TP_PCH_GPP_E_5
    {GPIO_SKL_H_GPP_E6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone}},//GPP_E_6_TP_PCH_GPP_E_6
    {GPIO_SKL_H_GPP_E7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_7_FM_ADR_SMI_GPIO_N
    {GPIO_SKL_H_GPP_E8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_8_LED_PCH_SATA_HDD_N
    {GPIO_SKL_H_GPP_E9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_9_FM_OC0_USB_N
    {GPIO_SKL_H_GPP_E10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_10_FM_OC1_USB_N
    {GPIO_SKL_H_GPP_E11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_11_FM_OC2_USB_N
    {GPIO_SKL_H_GPP_E12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_12_FM_OC3_USB_N
    {GPIO_SKL_H_GPP_F0,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_0_TP_PCH_GPP_F_0
    {GPIO_SKL_H_GPP_F1,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_1_TP_PCH_GPP_F_1
    {GPIO_SKL_H_GPP_F2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_2_TP_PCH_GPP_F_2
    {GPIO_SKL_H_GPP_F3,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_3_TP_PCH_GPP_F_3
    {GPIO_SKL_H_GPP_F4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_4_TP_PCH_GPP_F_4
    {GPIO_SKL_H_GPP_F5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_F_5_IRQ_TPM_SPI_N
    {GPIO_SKL_H_GPP_F6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_F_6_JTAG_PCH_PLD_TCK
    {GPIO_SKL_H_GPP_F7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_F_7_JTAG_PCH_PLD_TDI
    {GPIO_SKL_H_GPP_F8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_F_8_JTAG_PCH_PLD_TMS
    {GPIO_SKL_H_GPP_F9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_F_9_JTAG_PCH_PLD_TDO
    {GPIO_SKL_H_GPP_F10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_10_SGPIO_SATA_CLOCK
    {GPIO_SKL_H_GPP_F11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_11_SGPIO_SATA_LOAD
    {GPIO_SKL_H_GPP_F12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_12_SGPIO_SATA_DATA1
    {GPIO_SKL_H_GPP_F13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_13_SGPIO_SATA_DATA0
    {GPIO_SKL_H_GPP_F14, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_14_LED_PCH_SSATA_HDD_N
    {GPIO_SKL_H_GPP_F15, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_15_FM_OC4_USB_N
    {GPIO_SKL_H_GPP_F16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_16_FM_OC5_USB_N
    {GPIO_SKL_H_GPP_F17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_17_FM_OC6_USB_N
    {GPIO_SKL_H_GPP_F18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_18_FM_OC7_USB_N
    {GPIO_SKL_H_GPP_F19, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_19_SMB_GBE_STBY_LVC3_SCL
    {GPIO_SKL_H_GPP_F20, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_20_SMB_GBE_STBY_LVC3_SDA
    {GPIO_SKL_H_GPP_F21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_21_TP_PCH_GPP_F_21
    {GPIO_SKL_H_GPP_F22, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_22_SGPIO_SSATA_CLOCK
    {GPIO_SKL_H_GPP_F23, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_23_SGPIO_SSATA_LOAD
    {GPIO_SKL_H_GPP_G0,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_G1,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_G2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_G3,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_G4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_G5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_G6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_G7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_G8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_G9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_G10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_G11, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_G12, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_G_12_FM_BOARD_SKU_ID0
    {GPIO_SKL_H_GPP_G13, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_G_13_FM_BOARD_SKU_ID1
    {GPIO_SKL_H_GPP_G14, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_G_14_FM_BOARD_SKU_ID2
    {GPIO_SKL_H_GPP_G15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_G_15_FM_BOARD_SKU_ID3
    {GPIO_SKL_H_GPP_G16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_G17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_17_ADR_COMPLETE
    {GPIO_SKL_H_GPP_G18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_18_FM_NMI_EVENT_N
    {GPIO_SKL_H_GPP_G19, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_19_FM_SMI_ACTIVE_N
    {GPIO_SKL_H_GPP_G21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_G_21_FM_SATAEXPRESS_DEVSLP
    {GPIO_SKL_H_GPP_G23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_23_FM_SSATA_PCIE_SEL
    {GPIO_SKL_H_GPP_H0,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_H_0_PU_PCH_GPP_H_0
    {GPIO_SKL_H_GPP_H2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_H3,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_H4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_H6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_H7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_H8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_H9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_H12, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_H_12_FM_ESPI_FLASH_MODE
    {GPIO_SKL_H_GPP_H15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_H_15_PU_ADR_TIMER_HOLD_OFF_N
    {GPIO_SKL_H_GPP_H18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_H_18_FM_LT_KEY_DOWNGRADE_N
    {GPIO_SKL_H_GPP_H19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_H_19_TP_PCH_GPP_H_19
    {GPIO_SKL_H_GPP_H20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//SMCPKG_SUPPORT
    {GPIO_SKL_H_GPP_H21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_H_21_FM_PCH_MGPIO_TEST3
    {GPIO_SKL_H_GPP_H22, { GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//GPP_H_22_FM_SSATA_PCIE_M2_SEL
    {GPIO_SKL_H_GPP_H23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone, GpioPadConfigLock}},//TP_PCH_GPP_H_23
    {GPIO_SKL_H_GPP_I0,  { GpioPadModeNative2, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_0_GBE_TDO
    {GPIO_SKL_H_GPP_I1,  { GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_1_GBE_TCK
    {GPIO_SKL_H_GPP_I2,  { GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_2_GBE_TMS
    {GPIO_SKL_H_GPP_I3,  { GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_3_GBE_TDI
    {GPIO_SKL_H_GPP_I4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_I_4_FP_LED_STATUS_GREEN_PCH_N
    {GPIO_SKL_H_GPP_I5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_I_5_FP_LED_STATUS_AMBER_PCH_N
    {GPIO_SKL_H_GPP_I6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_I_6_FP_ID_LED_PCH_N
    {GPIO_SKL_H_GPP_I7,  { GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_7_JTAG_GBE_TRST_N
    {GPIO_SKL_H_GPP_I8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_8_FP_ID_BTN_PCH_N
    {GPIO_SKL_H_GPP_I9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_9_FM_MEM_THERM_EVENT_PCH_N
    {GPIO_SKL_H_GPP_I10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_10_TP_PCH_GPP_I_10
    {GPIO_SKL_H_GPD1,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_1_ACPRESENT
    {GPIO_SKL_H_GPD2,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_2_LAN_WAKEB
    {GPIO_SKL_H_GPD3,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_3_PWRBTNB
    {GPIO_SKL_H_GPD4,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_4_SLP_S3B
    {GPIO_SKL_H_GPD5,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_5_SLP_S4B
    {GPIO_SKL_H_GPD6,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_6_SLP_AB
    {GPIO_SKL_H_GPD7,    { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPD_7_TP_GPD_7
    {GPIO_SKL_H_GPD8,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_8_CLK_33K_PCH_SUSCLK_PLD
    {GPIO_SKL_H_GPD9,    { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPD_9_TP_SLP_WLAN
    {GPIO_SKL_H_GPD10,   { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_10_SLP_S5B_N
    {GPIO_SKL_H_GPD11,   { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_11_FM_PHY_DISABLE_N
    {GPIO_SKL_H_GPP_J1,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_1_LED_GBE_0_SPEED
    {GPIO_SKL_H_GPP_J2,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_2_LED_GBE_1_ACTIVITY
    {GPIO_SKL_H_GPP_J3,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_3_LED_GBE_1_SPEED
    {GPIO_SKL_H_GPP_J4,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_4_LED_GBE_2_ACTIVITY
    {GPIO_SKL_H_GPP_J5,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_5_LED_GBE_2_SPEED
    {GPIO_SKL_H_GPP_J6,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_6_LED_GBE_3_ACTIVITY
    {GPIO_SKL_H_GPP_J7,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_7_LED_GBE_3_SPEED
    {GPIO_SKL_H_GPP_J8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_8_SMB_GBE0_LVC3_R_SCL
    {GPIO_SKL_H_GPP_J9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_9_SMB_GBE0_LVC3_R_SDA
    {GPIO_SKL_H_GPP_J10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_10_SMB_GBE1_LVC3_R_SCL
    {GPIO_SKL_H_GPP_J11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_11_SMB_GBE1_LVC3_R_SDA
    {GPIO_SKL_H_GPP_J12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_12_SMB_GBE2_LVC3_R_SCL
    {GPIO_SKL_H_GPP_J13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_13_SMB_GBE2_LVC3_R_SDA
    {GPIO_SKL_H_GPP_J14, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_14_SMB_GBE3_LVC3_R_SCL
    {GPIO_SKL_H_GPP_J15, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_15_SMB_GBE3_LVC3_R_SDA
    {GPIO_SKL_H_GPP_J16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_16_FM_T1_LVC3_MOD_ABS0
    {GPIO_SKL_H_GPP_J17, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_J_17_LED_GBE_0_LOW_SPEED
    {GPIO_SKL_H_GPP_J18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_18_FM_L1_LVC3_MOD_ABS0
    {GPIO_SKL_H_GPP_J19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_J_19_LED_GBE_1_LOW_SPEED
    {GPIO_SKL_H_GPP_J20, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_20_FM_T2_LVC3_MOD_ABS0
    {GPIO_SKL_H_GPP_J21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_J_21_LED_GBE_2_LOW_SPEED
    {GPIO_SKL_H_GPP_J22, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_22_FM_L2_LVC3_MOD_ABS0
    {GPIO_SKL_H_GPP_J23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_J_23_LED_GBE_3_LOW_SPEED
    {GPIO_SKL_H_GPP_K0,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_0_CLK_50M_CKMNG_PCH
    {GPIO_SKL_H_GPP_K1,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_1_RMII_PCH_BMC_RXD<0>
    {GPIO_SKL_H_GPP_K2,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_2_RMII_PCH_BMC_RXD<1>
    {GPIO_SKL_H_GPP_K3,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_3_GBE_NCSI_CRS_DV_R
    {GPIO_SKL_H_GPP_K4,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_4_RMII_BMC_PCH_TX_EN
    {GPIO_SKL_H_GPP_K5,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_5_RMII_BMC_PCH_TXD<0>
    {GPIO_SKL_H_GPP_K6,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_6_RMII_BMC_PCH_TXD<1>
    {GPIO_SKL_H_GPP_K7,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirInOut, GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_7_RMII_PCH_BMC_RX_ER
    {GPIO_SKL_H_GPP_K8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_8_PD_RMII_PCH_ARB_IN
    {GPIO_SKL_H_GPP_K9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_9_PU_RMII_PCH_ARB_OUT
    {GPIO_SKL_H_GPP_K10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_10_RST_PCIE_PCH_PERST_N
    {GPIO_SKL_H_GPP_L2,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_2_VISA2CH0_D0
    {GPIO_SKL_H_GPP_L3,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_3_VISA2CH0_D1
    {GPIO_SKL_H_GPP_L4,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_4_VISA2CH0_D2
    {GPIO_SKL_H_GPP_L5,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_5_VISA2CH0_D3
    {GPIO_SKL_H_GPP_L6,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_6_VISA2CH0_D4
    {GPIO_SKL_H_GPP_L7,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_7_VISA2CH0_D5
    {GPIO_SKL_H_GPP_L8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_8_VISA2CH0_D6
    {GPIO_SKL_H_GPP_L9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_9_VISA2CH0_D7
    {GPIO_SKL_H_GPP_L10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_10_VISA2CH0_CLK
    {GPIO_SKL_H_GPP_L11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_11_VISA2CH1_D0
    {GPIO_SKL_H_GPP_L12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_12_VISA2CH1_D1
    {GPIO_SKL_H_GPP_L13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_13_VISA2CH1_D2
    {GPIO_SKL_H_GPP_L14, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_14_VISA2CH1_D3
    {GPIO_SKL_H_GPP_L15, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_15_VISA2CH1_D4
    {GPIO_SKL_H_GPP_L16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_16_VISA2CH1_D5
    {GPIO_SKL_H_GPP_L17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_17_VISA2CH1_D6
    {GPIO_SKL_H_GPP_L18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_18_VISA2CH1_D7
    {GPIO_SKL_H_GPP_L19, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_19_VISA2CH1_CLK
};

EFI_STATUS
TypeNeonCityEPRPInstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
)
{
  EFI_STATUS                            Status;
if (PcdGet16(PcdOemSkuSubBoardID) == TypeBarkPeak) {
  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformGpioInitDataGuid, 
                                 &mGpioTableBarkPeakEP, 
                                 sizeof(mGpioTableBarkPeakEP)
                                 );
  Status = PcdSet32S(PcdOemSku_GPIO_TABLE_SIZE,sizeof(mGpioTableBarkPeakEP));
  ASSERT_EFI_ERROR(Status);
} else {
  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformGpioInitDataGuid, 
                                 &mGpioTableNeonCityEP, 
                                 sizeof(mGpioTableNeonCityEP)
                                 );
  Status = PcdSet32S(PcdOemSku_GPIO_TABLE_SIZE,sizeof(mGpioTableNeonCityEP));
  ASSERT_EFI_ERROR(Status);
}
  return Status;
}
