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

static GPIO_INIT_CONFIG mGpioTableLightningRidgeEX[] =
{
  {GPIO_SKL_H_GPP_A0,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_0_RCINB_ESPI_ALERT1B
  {GPIO_SKL_H_GPP_A1,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_1_LAD_0_ESPI_IO_0
  {GPIO_SKL_H_GPP_A2,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_2_LAD_1_ESPI_IO_1
  {GPIO_SKL_H_GPP_A3,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_3_LAD_2_ESPI_IO_2
  {GPIO_SKL_H_GPP_A4,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_4_LAD_3_ESPI_IO_3
  {GPIO_SKL_H_GPP_A5,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_5_LFRAMEB_ESPI_CS0B
  {GPIO_SKL_H_GPP_A6,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_6_SERIRQ_ESPI_CS1B
  {GPIO_SKL_H_GPP_A7,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_7_PIRQAB_ESPI_ALERT0B
  {GPIO_SKL_H_GPP_A8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_8_CLKRUNB
  {GPIO_SKL_H_GPP_A9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_9_CLKOUT_LPC_0_ESPI_CLK
  {GPIO_SKL_H_GPP_A10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_10_CLKOUT_LPC_1
  {GPIO_SKL_H_GPP_A11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_11_PMEB
  {GPIO_SKL_H_GPP_A12, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv,    GpioOutDefault, GpioIntLevel | GpioIntSci,GpioResetNormal,  GpioTermNone}},//GPP_A_12_BM_BUSYB_SXEXITHLDOFFB
  {GPIO_SKL_H_GPP_A13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_13_SUSWARNB_SUSPWRDNACK
  {GPIO_SKL_H_GPP_A14, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_14_ESPI_RESETB
  {GPIO_SKL_H_GPP_A15, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_15_SUSACKB
  {GPIO_SKL_H_GPP_A16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_16_CLKOUT_LPC_2
  {GPIO_SKL_H_GPP_A17, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_17
  {GPIO_SKL_H_GPP_A18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_A_18
  {GPIO_SKL_H_GPP_A20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_A_20
  {GPIO_SKL_H_GPP_A21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_A_21
  {GPIO_SKL_H_GPP_A22, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_22
  {GPIO_SKL_H_GPP_A23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_A_23
  {GPIO_SKL_H_GPP_B0,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_0_CORE_VID_0
  {GPIO_SKL_H_GPP_B1,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_1_CORE_VID_1
  {GPIO_SKL_H_GPP_B2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_2_VRALERTB
  {GPIO_SKL_H_GPP_B3,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_3_CPU_GP_2
  {GPIO_SKL_H_GPP_B4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_4_CPU_GP_3
  {GPIO_SKL_H_GPP_B5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_5_SRCCLKREQB_0
  {GPIO_SKL_H_GPP_B6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_B_6_SRCCLKREQB_1
  {GPIO_SKL_H_GPP_B7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_B_7_SRCCLKREQB_2
  {GPIO_SKL_H_GPP_B8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_8_SRCCLKREQB_3
  {GPIO_SKL_H_GPP_B9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_9_SRCCLKREQB_4
  {GPIO_SKL_H_GPP_B10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_10_SRCCLKREQB_5
  {GPIO_SKL_H_GPP_B12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_12_SLP_S0B
  {GPIO_SKL_H_GPP_B13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_13_PLTRSTB
  {GPIO_SKL_H_GPP_B14, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone}},//GPP_B_14_SPKR
  {GPIO_SKL_H_GPP_B15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_15
  {GPIO_SKL_H_GPP_B16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_16
  {GPIO_SKL_H_GPP_B17, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_17
  {GPIO_SKL_H_GPP_B18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_B_18
  {GPIO_SKL_H_GPP_B19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_19
  {GPIO_SKL_H_GPP_B20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_B_20
  {GPIO_SKL_H_GPP_B21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_21
  {GPIO_SKL_H_GPP_B22, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_B_22
  {GPIO_SKL_H_GPP_B23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_B_23MEIE_SML1ALERTB_PCHHOTB
  {GPIO_SKL_H_GPP_C8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_C_8
  {GPIO_SKL_H_GPP_C9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_C_9
  {GPIO_SKL_H_GPP_C10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone}},//GPP_C_10
  {GPIO_SKL_H_GPP_C11, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_C_11
  {GPIO_SKL_H_GPP_C12, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_C_12
  {GPIO_SKL_H_GPP_C13, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_C_13
  {GPIO_SKL_H_GPP_C14, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSci,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_C_14
  {GPIO_SKL_H_GPP_C15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_C_15
  {GPIO_SKL_H_GPP_C16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_C_16
  {GPIO_SKL_H_GPP_C17, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_C_17
  {GPIO_SKL_H_GPP_C18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_C_18
  {GPIO_SKL_H_GPP_C19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_C_19
  {GPIO_SKL_H_GPP_C22, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel | GpioIntSmi,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_C_22
  {GPIO_SKL_H_GPP_C23, { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntSmi,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_C_23
  {GPIO_SKL_H_GPP_D0,  { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntNmi,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_D_0
  {GPIO_SKL_H_GPP_D1,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_D_1
  {GPIO_SKL_H_GPP_D2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_D_2
  {GPIO_SKL_H_GPP_D3,  { GpioPadModeGpio,    GpioHostOwnAcpi, GpioDirNone,  GpioOutDefault, GpioIntLevel | GpioIntSci,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_D_3
  {GPIO_SKL_H_GPP_D4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_D_4
  {GPIO_SKL_H_GPP_D5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_5_SSP0_SFRM
  {GPIO_SKL_H_GPP_D6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_6_SSP0_TXD
  {GPIO_SKL_H_GPP_D7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_7_SSP0_RXD
  {GPIO_SKL_H_GPP_D8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_8_SSP0_SCLK
  {GPIO_SKL_H_GPP_D9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_9_SSATA_DEVSLP_3
  {GPIO_SKL_H_GPP_D10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_10_SSATA_DEVSLP_4
  {GPIO_SKL_H_GPP_D11, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_D_11_SSATA_DEVSLP_5
  {GPIO_SKL_H_GPP_D12, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_12_SSATA_SDATAOUT1
  {GPIO_SKL_H_GPP_D13, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_13_SML0BCLK_SML0BCLKIE
  {GPIO_SKL_H_GPP_D14, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_14_SML0BDATA_SML0BDATAIE
  {GPIO_SKL_H_GPP_D15, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_15_SSATA_SDATAOUT0
  {GPIO_SKL_H_GPP_D16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_D_16_SML0BALERTB_SML0BALERTBIE
  {GPIO_SKL_H_GPP_D17, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_D_17_DMIC_CLK_1
  {GPIO_SKL_H_GPP_D18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_D_18_DMIC_DATA_1
  {GPIO_SKL_H_GPP_D19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_D_19_DMIC_CLK_0
  {GPIO_SKL_H_GPP_D20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_20_DMIC_DATA_0
  {GPIO_SKL_H_GPP_D21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_21_IE_UART_RXD
  {GPIO_SKL_H_GPP_D22, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_22_IE_UART_TXD
  {GPIO_SKL_H_GPP_D23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_D_23
  {GPIO_SKL_H_GPP_E0,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_0_SATAXPCIE_0_SATAGP_0
  {GPIO_SKL_H_GPP_E1,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_1_SATAXPCIE_1_SATAGP_1
  {GPIO_SKL_H_GPP_E2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_2_SATAXPCIE_2_SATAGP_2
  {GPIO_SKL_H_GPP_E3,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_3_CPU_GP_0
  {GPIO_SKL_H_GPP_E4,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone}},//GPP_E_4_SATA_DEVSLP_0
  {GPIO_SKL_H_GPP_E5,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone}},//GPP_E_5_SATA_DEVSLP_1
  {GPIO_SKL_H_GPP_E6,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone}},//GPP_E_6_SATA_DEVSLP_2
  {GPIO_SKL_H_GPP_E7,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_7_CPU_GP_1
  {GPIO_SKL_H_GPP_E8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_8_SATA_LEDB
  {GPIO_SKL_H_GPP_E9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_9_USB2_OCB_0
  {GPIO_SKL_H_GPP_E10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_10_USB2_OCB_1
  {GPIO_SKL_H_GPP_E11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_11_USB2_OCB_2
  {GPIO_SKL_H_GPP_E12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_E_12_USB2_OCB_3
  {GPIO_SKL_H_GPP_F0,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_0_SATAXPCIE_3_SATAGP_3
  {GPIO_SKL_H_GPP_F1,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_1_SATAXPCIE_4_SATAGP_4
  {GPIO_SKL_H_GPP_F2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_2_SATAXPCIE_5_SATAGP_5
  {GPIO_SKL_H_GPP_F3,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_3_SATAXPCIE_6_SATAGP_6
  {GPIO_SKL_H_GPP_F4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_4_SATAXPCIE_7_SATAGP_7
  {GPIO_SKL_H_GPP_F5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel | GpioIntApic,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_F_5_SATA_DEVSLP_3
  {GPIO_SKL_H_GPP_F6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_F_6_SATA_DEVSLP_4
  {GPIO_SKL_H_GPP_F7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_F_7_SATA_DEVSLP_5
  {GPIO_SKL_H_GPP_F8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_F_8_SATA_DEVSLP_6
  {GPIO_SKL_H_GPP_F9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_F_9_SATA_DEVSLP_7
  {GPIO_SKL_H_GPP_F10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_10_SATA_SCLOCK
  {GPIO_SKL_H_GPP_F11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_11_SATA_SLOAD
  {GPIO_SKL_H_GPP_F12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_12_SATA_SDATAOUT1
  {GPIO_SKL_H_GPP_F13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_13_SATA_SDATAOUT0
  {GPIO_SKL_H_GPP_F14, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_14_SSATA_LEDB
  {GPIO_SKL_H_GPP_F15, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_15_USB2_OCB_4
  {GPIO_SKL_H_GPP_F16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_16_USB2_OCB_5
  {GPIO_SKL_H_GPP_F17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_17_USB2_OCB_6
  {GPIO_SKL_H_GPP_F18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_18_USB2_OCB_7
  {GPIO_SKL_H_GPP_F19, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_19_GBE_SMBUS_CLK
  {GPIO_SKL_H_GPP_F20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_20_GBE_SMBDATA
  {GPIO_SKL_H_GPP_F21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_21_GBE_SMBALRTN
  {GPIO_SKL_H_GPP_F22, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_22_SSATA_SCLOCK
  {GPIO_SKL_H_GPP_F23, { GpioPadModeNative3, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_F_23_SSATA_SLOAD
  {GPIO_SKL_H_GPP_G0,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_0_FAN_TACH_0_FAN_TACH0IE
  {GPIO_SKL_H_GPP_G1,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_1_FAN_TACH_1_FAN_TACH1IE
  {GPIO_SKL_H_GPP_G2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_2_FAN_TACH_2_FAN_TACH2IE
  {GPIO_SKL_H_GPP_G3,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_3_FAN_TACH_3_FAN_TACH3IE
  {GPIO_SKL_H_GPP_G4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_4_FAN_TACH_4_FAN_TACH4IE
  {GPIO_SKL_H_GPP_G5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_5_FAN_TACH_5_FAN_TACH5IE
  {GPIO_SKL_H_GPP_G6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_6_FAN_TACH_6_FAN_TACH6IE
  {GPIO_SKL_H_GPP_G7,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_7_FAN_TACH_7_FAN_TACH7IE
  {GPIO_SKL_H_GPP_G8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_G_8_FAN_PWM_0_FAN_PWM0IE
  {GPIO_SKL_H_GPP_G9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_G_9_FAN_PWM_1_FAN_PWM1IE
  {GPIO_SKL_H_GPP_G10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_G_10_FAN_PWM_2_FAN_PWM2IE
  {GPIO_SKL_H_GPP_G11, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_G_11_FAN_PWM_3_FAN_PWM3IE
  {GPIO_SKL_H_GPP_G12, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_G_12_GSXDOUT
  {GPIO_SKL_H_GPP_G13, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_G_13_GSXSLOAD
  {GPIO_SKL_H_GPP_G14, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_G_14_GSXDIN
  {GPIO_SKL_H_GPP_G15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_G_15_GSXSRESETB
  {GPIO_SKL_H_GPP_G16, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_G_16_GSXCLK
  {GPIO_SKL_H_GPP_G17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_17_ADR_COMPLETE
  {GPIO_SKL_H_GPP_G18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_18_NMIB
  {GPIO_SKL_H_GPP_G19, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_19_SMIB
  {GPIO_SKL_H_GPP_G23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_G_23_SSATAXPCIE0_SSATAGP0
  {GPIO_SKL_H_GPP_H0,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_H_0_SRCCLKREQB_6
  {GPIO_SKL_H_GPP_H2,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_H_2_SRCCLKREQB_8
  {GPIO_SKL_H_GPP_H3,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_H_3_SRCCLKREQB_9
  {GPIO_SKL_H_GPP_H12, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_H_12_SML2ALERTB_SML2ALERTBIE
  {GPIO_SKL_H_GPP_H15, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_H_15_SML3ALERTB_SML3ALERTBIE
  {GPIO_SKL_H_GPP_H18, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock}},//GPP_H_18_SML4ALERTB_SML4ALERTBIE
  {GPIO_SKL_H_GPP_H19, { GpioPadModeNative2, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,  GpioTermWpu5K, GpioPadConfigLock}},//GPP_H_19_SSATAXPCIE1_SSATAGP1
  {GPIO_SKL_H_GPP_H20, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_H_20_SSATAXPCIE2_SSATAGP2
  {GPIO_SKL_H_GPP_H21, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_H_21_SSATAXPCIE3_SSATAGP3
  {GPIO_SKL_H_GPP_H22, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_H_22_SSATAXPCIE4_SSATAGP4
  {GPIO_SKL_H_GPP_H23, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,GpioResetNormal,  GpioTermNone, GpioPadConfigLock | GpioOutputStateLock}},//GPP_H_23_SSATAXPCIE5_SSATAGP5
  {GPIO_SKL_H_GPP_I0,  { GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_0_GBE_TDO
  {GPIO_SKL_H_GPP_I1,  { GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_1_GBE_TCK
  {GPIO_SKL_H_GPP_I2,  { GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_2_GBE_TMS
  {GPIO_SKL_H_GPP_I3,  { GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_3_GBE_TDI
  {GPIO_SKL_H_GPP_I4,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_4_DO_RESET_INB
  {GPIO_SKL_H_GPP_I5,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_5_DO_RESET_OUTB
  {GPIO_SKL_H_GPP_I6,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_6_RESET_DONE
  {GPIO_SKL_H_GPP_I7,  { GpioPadModeNative2, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_7_GBE_TRST_N
  {GPIO_SKL_H_GPP_I8,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_8_GBE_PCI_DIS
  {GPIO_SKL_H_GPP_I9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_9_GBE_LAN_DIS
  {GPIO_SKL_H_GPP_I10, { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_I_10
  {GPIO_SKL_H_GPD1,    { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_1_ACPRESENT
  {GPIO_SKL_H_GPD2,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_2_LAN_WAKEB
  {GPIO_SKL_H_GPD3,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_3_PWRBTNB
  {GPIO_SKL_H_GPD4,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_4_SLP_S3B
  {GPIO_SKL_H_GPD5,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_5_SLP_S4B
  {GPIO_SKL_H_GPD6,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_6_SLP_AB
  {GPIO_SKL_H_GPD7,    { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutHigh,    GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_7_USB2_WAKEOUTB
  {GPIO_SKL_H_GPD8,    { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_8_SUSCLK
  {GPIO_SKL_H_GPD9,    { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_9_SLP_WLANB
  {GPIO_SKL_H_GPD10,   { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_10_SLP_S5B
  {GPIO_SKL_H_GPD11,   { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetPwrGood,  GpioTermNone, GpioPadConfigLock}},//GPD_11_LANPHYPC
  {GPIO_SKL_H_GPP_J1,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_1_GBE_LED_0_1
  {GPIO_SKL_H_GPP_J2,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_2_GBE_LED_1_0
  {GPIO_SKL_H_GPP_J3,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_3_GBE_LED_1_1
  {GPIO_SKL_H_GPP_J4,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_4_GBE_LED_2_0
  {GPIO_SKL_H_GPP_J5,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_5_GBE_LED_2_1
  {GPIO_SKL_H_GPP_J6,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_6_GBE_LED_3_0
  {GPIO_SKL_H_GPP_J7,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_7_GBE_LED_3_1
  {GPIO_SKL_H_GPP_J8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_8_GBE_SCL_0
  {GPIO_SKL_H_GPP_J9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_9_GBE_SDA_0
  {GPIO_SKL_H_GPP_J10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_10_GBE_SCL_1
  {GPIO_SKL_H_GPP_J11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_11_GBE_SDA_1
  {GPIO_SKL_H_GPP_J12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_12_GBE_SCL_2
  {GPIO_SKL_H_GPP_J13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_13_GBE_SDA_2
  {GPIO_SKL_H_GPP_J14, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_14_GBE_SCL_3
  {GPIO_SKL_H_GPP_J15, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_15_GBE_SDA_3
  {GPIO_SKL_H_GPP_J16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_16_GBE_SDP_0_0
  {GPIO_SKL_H_GPP_J17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_17_GBE_SDP_0_1
  {GPIO_SKL_H_GPP_J18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_18_GBE_SDP_1_0
  {GPIO_SKL_H_GPP_J19, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_19_GBE_SDP_1_1
  {GPIO_SKL_H_GPP_J20, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_20_GBE_SDP_2_0
  {GPIO_SKL_H_GPP_J21, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_21_GBE_SDP_2_1
  {GPIO_SKL_H_GPP_J22, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_22_GBE_SDP_3_0
  {GPIO_SKL_H_GPP_J23, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_J_23_GBE_SDP_3_1
  {GPIO_SKL_H_GPP_K0,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_0_GBE_RMIICLK
  {GPIO_SKL_H_GPP_K1,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_1_GBE_RMII_RXD_0
  {GPIO_SKL_H_GPP_K2,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_2_GBE_RMII_RXD_1
  {GPIO_SKL_H_GPP_K3,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_3_GBE_RMII_CRS_DV
  {GPIO_SKL_H_GPP_K4,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_4_GBE_RMII_TX_EN
  {GPIO_SKL_H_GPP_K5,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_5_GBE_RMII_TXD_0
  {GPIO_SKL_H_GPP_K6,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_6_GBE_RMII_TXD_1
  {GPIO_SKL_H_GPP_K7,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_7_GBE_RMII_RX_ER
  {GPIO_SKL_H_GPP_K8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_8_GBE_RMII_ARBIN
  {GPIO_SKL_H_GPP_K9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_9_GBE_RMII_ARB_OUT
  {GPIO_SKL_H_GPP_K10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_K_10_PE_RST_N
  {GPIO_SKL_H_GPP_L2,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_2_VISA2CH0_D0
  {GPIO_SKL_H_GPP_L3,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_3_VISA2CH0_D1
  {GPIO_SKL_H_GPP_L4,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_4_VISA2CH0_D2
  {GPIO_SKL_H_GPP_L5,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_5_VISA2CH0_D3
  {GPIO_SKL_H_GPP_L6,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_6_VISA2CH0_D4
  {GPIO_SKL_H_GPP_L7,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_7_VISA2CH0_D5
  {GPIO_SKL_H_GPP_L8,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_8_VISA2CH0_D6
  {GPIO_SKL_H_GPP_L9,  { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_9_VISA2CH0_D7
  {GPIO_SKL_H_GPP_L10, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_10_VISA2CH0_CLK
  {GPIO_SKL_H_GPP_L11, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_11_VISA2CH1_D0
  {GPIO_SKL_H_GPP_L12, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_12_VISA2CH1_D1
  {GPIO_SKL_H_GPP_L13, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_13_VISA2CH1_D2
  {GPIO_SKL_H_GPP_L14, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_14_VISA2CH1_D3
  {GPIO_SKL_H_GPP_L15, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_15_VISA2CH1_D4
  {GPIO_SKL_H_GPP_L16, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_16_VISA2CH1_D5
  {GPIO_SKL_H_GPP_L17, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_17_VISA2CH1_D6
  {GPIO_SKL_H_GPP_L18, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_18_VISA2CH1_D7
  {GPIO_SKL_H_GPP_L19, { GpioPadModeNative1, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone, GpioPadConfigLock}},//GPP_L_19_VISA2CH1_CLK
};

EFI_STATUS
TypeLightningRidgeEXECB3InstallGpioData (
  IN UBA_CONFIG_DATABASE_PPI    *UbaConfigPpi
)
{
  EFI_STATUS                            Status;
  
  Status = UbaConfigPpi->AddData (
                                 UbaConfigPpi, 
                                 &gPlatformGpioInitDataGuid, 
                                 &mGpioTableLightningRidgeEX, 
                                 sizeof(mGpioTableLightningRidgeEX)
                                 );
  Status = PcdSet32S(PcdOemSku_GPIO_TABLE_SIZE,sizeof(mGpioTableLightningRidgeEX));
  ASSERT_EFI_ERROR(Status);
  return Status;
}
