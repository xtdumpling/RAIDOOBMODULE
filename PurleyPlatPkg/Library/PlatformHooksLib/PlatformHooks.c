//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fixed:  Fixed the swjpme2 GPIO(GPP_C9) attribute would be chagned problem.
//    Reason:     Rewrite the check ME state function in IsManufacturingMode.
//    Auditor:    Jimmy Chiu
//    Date:       Sep/26/2016
//
//****************************************************************************
//****************************************************************************
/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


    @file PlatformHooks.c

    Platform Hooks file

**/
#include <Base.h>
#include <Register/PchRegsSpi.h>
#include <Library/MmPciBaseLib.h>
#include <PchAccess.h>
#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>
#include <Platform.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

// APTIOV_SERVER_OVERRIDE_RC_START : Added Oem verb table data
#include <Ppi/PchHdaVerbTable.h>

#if SMCPKG_SUPPORT
#if JPME2_Jumpless_SUPPORT
#include <Sps.h>
#include "MeAccess.h"
#endif
#endif

HDAUDIO_VERB_TABLE OnBoardOemVertable = {
  // Please add oem verb table here

  //=== Please do not change below the line ===
    {
        0xFFFF,     // Vendor ID
        0xFFFF,     // Device ID
        0xFF,       // Revision ID (any)
        0xFF,       // SDI number (any)
        0xFF,       // Data size in DWORDs
//        0
    },
    {0}
};

HDAUDIO_VERB_TABLE ExtKitOemVertable = {
  // Please add oem verb table here

  //=== Please do not change below the line ===
    {
        0xFFFF,     // Vendor ID
        0xFFFF,     // Device ID
        0xFF,       // Revision ID (any)
        0xFF,       // SDI number (any)
        0xFF,       // Data size in DWORDs
//        0
    },
    {0}
};
// APTIOV_SERVER_OVERRIDE_RC_END : Added Oem verb table data

typedef union BOARD_ID
{
   struct{
      UINT8  BoardID0            :1;
      UINT8  BoardID1            :1;
      UINT8  BoardID2            :1;
      UINT8  BoardID3            :1;
      UINT8  BoardID4            :1;
      UINT8  BoardRev0           :1;
      UINT8  BoardRev1           :1;
      UINT8  Rsvd                :1;
   }BoardID;
}BOARD_ID;

BOOLEAN
IsSimicsPlatform(VOID)
{
  UINT8        EmulationType;

  EmulationType = PciRead8 ( PCI_LIB_ADDRESS(0, 0, 0, 0xFC));

  if (EmulationType == BIT2) {
    return TRUE;
  } else {
    return FALSE;
  }
}

UINT8
IsSimPlatform(VOID)
{
  UINT8        i;
  UINT8        EmulationType = 0xff;

  //CSR_EMULATION_FLAG_1_16_5_CFG_REG
  for (i = 0; i < MAX_SOCKET; i++) {
    EmulationType = PciRead8 ( PCI_LIB_ADDRESS((i*0x40 + 0x3F), 16, 5, 0xb0));
    if (EmulationType != 0xff)
      break;
  }

  if (EmulationType != 0xff) {
    EmulationType &= 7;
  } else {
    EmulationType = 0;
  }

  return EmulationType;
}

VOID
GPIOConfigForBoardId(
  VOID
  )
{
  UINT32               Dw0Reg = 0;
  UINT32               Dw0RegMask = 0;
  UINT32               Dw1Reg = 0;
  UINT32               Dw1RegMask = 0;
  UINT32               PadCfgReg;
  UINT8                i;

  // Configure Reset Type (PadRstCfg)
  Dw0RegMask |= (((GpioResetNormal & 0x7) == GpioHardwareDefault) ? 0x0 : B_PCH_GPIO_RST_CONF);
  Dw0Reg |= (((GpioResetNormal  & 0x7) >> 1) << N_PCH_GPIO_RST_CONF);
  // Configure how interrupt is triggered (RxEvCfg)
  Dw0RegMask |= (((GpioIntDis >> 5) == GpioHardwareDefault) ? 0x0 : B_PCH_GPIO_RX_LVL_EDG);
  Dw0Reg |= ((GpioIntDis >> 6) << N_PCH_GPIO_RX_LVL_EDG);
  // Configure interrupt generation (GPIRoutIOxAPIC/SCI/SMI/NMI)
  Dw0RegMask |= (((GpioIntDis & 0x1F)  == GpioHardwareDefault)  ? 0x0 : (B_PCH_GPIO_RX_NMI_ROUTE | B_PCH_GPIO_RX_SCI_ROUTE | B_PCH_GPIO_RX_SMI_ROUTE | B_PCH_GPIO_RX_APIC_ROUTE));
  Dw0Reg |= (((GpioIntDis & 0x1F) >> 1) << N_PCH_GPIO_RX_NMI_ROUTE);
  // Configure GPIO direction (GPIORxDis and GPIOTxDis)
  Dw0RegMask |= (((GpioDirIn & 0x7) == GpioHardwareDefault) ? 0x0 : (B_PCH_GPIO_RXDIS | B_PCH_GPIO_TXDIS));
  Dw0Reg |= (((GpioDirIn & 0x7) >> 1) << N_PCH_GPIO_TXDIS);
  // Configure GPIO input inversion (RXINV)
  Dw0RegMask |= (((GpioDirIn >> 3) == GpioHardwareDefault) ?  0x0 : B_PCH_GPIO_RXINV);
  Dw0Reg |= ((GpioDirIn >> 4) << N_PCH_GPIO_RXINV);
  // Configure GPIO output state (GPIOTxState)
  Dw0RegMask |= ((GpioOutHigh == GpioHardwareDefault) ? 0x0 : B_PCH_GPIO_TX_STATE);
  Dw0Reg |= ((GpioOutHigh >> 1) << N_PCH_GPIO_TX_STATE);
  // Configure GPIO Pad Mode (PMode)
  Dw0RegMask |= ((GpioPadModeGpio == GpioHardwareDefault) ? 0x0 : B_PCH_GPIO_PAD_MODE);
  Dw0Reg |= ((GpioPadModeGpio >> 1) << N_PCH_GPIO_PAD_MODE);
  // Configure GPIO termination (Term)
  Dw1RegMask |= (((GpioTermNone & 0x1F) == GpioHardwareDefault) ? 0x0 : B_PCH_GPIO_TERM);
  Dw1Reg |= (((GpioTermNone & 0x1F) >> 1) << N_PCH_GPIO_TERM);
  // Configure GPIO pad tolerance (padtol)
  Dw1RegMask |= (((GpioTermNone >> 5) == GpioHardwareDefault) ? 0x0 : B_PCH_GPIO_PADTOL);
  Dw1Reg |= ((GpioTermNone >> 6) << N_PCH_GPIO_PADTOL);

  for (i=0;i<5;i++) {
    // Calculate PAD configuration offset
    PadCfgReg = 0x8 * (V_GPIO_PAD12 + i) + R_PCH_H_PCR_GPIO_GPP_G_PADCFG_OFFSET;

    // Write PADCFG DW0 register
    MmioAndThenOr32 (
      (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM5, PadCfgReg),
      ~(UINT32)Dw0RegMask,
      (UINT32)Dw0Reg
    );

    // Write PADCFG DW1 register
    MmioAndThenOr32 (
      (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM5, PadCfgReg + 0x4),
      ~(UINT32)Dw1RegMask,
      (UINT32)Dw1Reg
    );

  }
}

/**

    Custom Table to set GPIO's for Boot Mode

**/
static GPIO_INIT_CONFIG mGpioCustomTable [] = 
{
    {GPIO_SKL_H_GPP_C9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,GpioResetNormal,GpioTermNone}},//GPP_C_9_FM_MFG_MODE
};

static GPIO_INIT_CONFIG mGpioCustomTable_NonMFG [] = 
{
    {GPIO_SKL_H_GPP_C9,  { GpioPadModeGpio,    GpioHostOwnGpio, GpioDirNone,  GpioOutDefault, GpioIntDis,GpioResetDeep,  GpioTermNone}},// Not used
};
/**

    Sets GPIO's used for Boot Mode

    @retval Status - Success if GPIO's are configured

**/
EFI_STATUS
GpioConfigForMFGMode(
  VOID
  )
{
  EFI_STATUS                  Status;
  UINT8                       BoardId;
  BOARD_ID                    Board_ID;
  UINT32                      Data32;

  ZeroMem(&Board_ID, sizeof(BOARD_ID));
  GPIOConfigForBoardId();
  DEBUG ((EFI_D_INFO, "Use GPIO to read Board ID\n"));

  Status = GpioGetInputValue (GPIO_SKL_H_GPP_G12, &Data32);
  if(EFI_ERROR(Status))
   return Status;
  if (Data32)
    Board_ID.BoardID.BoardID0 = 1;

  Status = GpioGetInputValue (GPIO_SKL_H_GPP_G13, &Data32);
  if(EFI_ERROR(Status))
   return Status;
  if (Data32)
    Board_ID.BoardID.BoardID1 = 1;

  Status = GpioGetInputValue (GPIO_SKL_H_GPP_G14, &Data32);
  if(EFI_ERROR(Status))
   return Status;
  if (Data32)
    Board_ID.BoardID.BoardID2 = 1;

  Status = GpioGetInputValue (GPIO_SKL_H_GPP_G15, &Data32);
  if(EFI_ERROR(Status))
   return Status;
  if (Data32)
    Board_ID.BoardID.BoardID3 = 1;

  Status = GpioGetInputValue (GPIO_SKL_H_GPP_G16, &Data32);
  if(EFI_ERROR(Status))
    return Status;
  if (Data32)
    Board_ID.BoardID.BoardID4 = 1;

  BoardId    = (UINT8)(Board_ID.BoardID.BoardID0
                      + (Board_ID.BoardID.BoardID1 << 1)
                      + (Board_ID.BoardID.BoardID2 << 2)
                      + (Board_ID.BoardID.BoardID3 << 3)
                      + (Board_ID.BoardID.BoardID4 << 4));

  DEBUG ((EFI_D_INFO, "Start ConfigureGpio() for BootMode Detection.\n"));
  DEBUG ((EFI_D_INFO, "Board ID = %x\n",BoardId));
  switch (BoardId) {
    case 0x03:  //for TypePurleyLBGEPDVP, no MFG Jumper Support
      Status = GpioConfigurePads (sizeof (mGpioCustomTable_NonMFG)/sizeof (GPIO_INIT_CONFIG), mGpioCustomTable_NonMFG);
      break;
    default:
      Status = GpioConfigurePads (sizeof (mGpioCustomTable)/sizeof (GPIO_INIT_CONFIG), mGpioCustomTable);
      break;
  }
  ASSERT_EFI_ERROR (Status);  
  DEBUG ((EFI_D_INFO, "End ConfigureGpio() for BootMode Detection.\n"));
  return Status;
}

/**

    GC_TODO: add routine description

    @param None

    @retval ManufacturingMode - GC_TODO: add retval description

**/
BOOLEAN
IsManufacturingMode(
  VOID
)
{
#if SMCPKG_SUPPORT == 0
  BOOLEAN ManufacturingMode = TRUE;

  EFI_STATUS Status;
  UINT32 GpiValue;

  Status = GpioConfigForMFGMode();
  ASSERT_EFI_ERROR (Status);  

  Status = GpioGetInputValue (GPIO_SKL_H_GPP_C9, &GpiValue);
  ASSERT_EFI_ERROR (Status);

  if (!GpiValue) {
    ManufacturingMode = FALSE;
  }

  return ManufacturingMode;
#else
#if JPME2_Jumpless_SUPPORT
    SPS_MEFS1  MeFs1;
    SPS_MEFS2  MeFs2;
    
    //
    // Note: We are checking if ME is in Recovery Mode actually.
    //       This follows the current status of Purley CRB.
    //       This may change in the future.
    //
    
    MeFs1.UInt32 = HeciPciRead32(0x40);
    if(MeFs1.UInt32 == 0xFFFFFFFF) MeFs1.UInt32 = 0;
    if (MeFs1.Bits.CurrentState != MEFS1_CURSTATE_RECOVERY) return FALSE;
    MeFs2.UInt32 = HeciPciRead32(0x48);
    if(MeFs2.UInt32 == 0xFFFFFFFF) MeFs2.UInt32 = 0;
    if (MeFs2.Bits.RecoveryCause != MEFS2_RCAUSE_MFGJMPR) return FALSE;
#endif    
    return TRUE;
#endif
}
