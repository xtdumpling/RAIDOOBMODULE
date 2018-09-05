//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Support SmcBoardType for FPGA.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Dec/27/2016
//
//  Rev. 1.00
//    Bug Fix:  Fix system hang 0x79.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/15/2016
//
//****************************************************************************
//****************************************************************************
/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file PlatformInfo.c

  Platform Info PEIM.

**/

#include "PlatformInfo.h"
#include <GpioPinsSklH.h>
#include <Library/GpioLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchGbeLib.h>
// APTIOV_SERVER_OVERRIDE_RC_START
//#include <Platform/SpiFvbServices/SpiFlashDeviceTable.h>  //Use AMI SPI services
#include <Library/AmiPlatformHooksLib.h>  //Use OEM functions from AmiPlatformHooksLib.h 
// APTIOV_SERVER_OVERRIDE_RC_END


#define  TEMP_BUS_NUMBER    (0x3F)

static EFI_PEI_PPI_DESCRIPTOR       mPlatformInfoPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPlatformInfoGuid,
    NULL
  };


/**

    GC_TODO: add routine description

    @param None

    @retval SioExit - GC_TODO: add retval description

**/
UINT32
IsSioExist (
  VOID
)
{
  //
  // IBMC will decode 0x4E/0x4F
  //
  UINT8   SioExit;
  UINT32  Delay;

  SioExit = 0;

  IoWrite8 (PILOTIV_SIO_INDEX_PORT, PILOTIV_SIO_UNLOCK);
  for (Delay = 0; Delay < 40; Delay++) IoRead8  (0x61);
  IoWrite8 (PILOTIV_SIO_INDEX_PORT, PILOTIV_LOGICAL_DEVICE);
  for (Delay = 0; Delay < 40; Delay++) IoRead8  (0x61);
  IoWrite8 (PILOTIV_SIO_DATA_PORT , PILOTIV_SIO_COM1);
  for (Delay = 0; Delay < 40; Delay++) IoRead8  (0x61);
  if (IoRead8  (PILOTIV_SIO_DATA_PORT) == PILOTIV_SIO_COM1) {
    SioExit |= PILOTIV_EXIST;
  }
  for (Delay = 0; Delay < 40; Delay++) IoRead8  (0x61);
  IoWrite8 (PILOTIV_SIO_INDEX_PORT, PILOTIV_SIO_LOCK);
  for (Delay = 0; Delay < 40; Delay++) IoRead8  (0x61);

  //
  // PC SIO will decode 0x2E/0x2F
  //
  IoWrite8 (PC8374_SIO_INDEX_PORT, PC8374_LOGICAL_DEVICE);
  IoWrite8 (PC8374_SIO_DATA_PORT , PC8374_SIO_COM1);
  if (IoRead8  (PC8374_SIO_DATA_PORT) == PC8374_SIO_COM1) {
    SioExit |= PC8374_EXIST;
  }

  return SioExit;
}

/**

   Returns the Model ID of the CPU.
   Model ID = EAX[7:4]

**/
VOID
GetCpuInfo (
  UINT32    *CpuType,
  UINT8     *CpuStepping
  )

{
  UINT32                    RegEax=0;

  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);

  *CpuStepping = (UINT8)(RegEax & 0x0F);
  *CpuType     = (UINT32)(RegEax >> 4);
}


/**

    GC_TODO: add routine description

    @param BAR         - GC_TODO: add arg description
    @param PeiServices - GC_TODO: add arg description

    @retval None

**/
VOID
InitGSX(
  UINT32                *BAR,
  IN EFI_PEI_SERVICES  **PeiServices
)
{
}

/**

    GC_TODO: add routine description

    @param Data        - GC_TODO: add arg description
    @param PeiServices - GC_TODO: add arg description

    @retval EFI_SUCCESS     - GC_TODO: add retval description
    @retval EFI_UNSUPPORTED - GC_TODO: add retval description

**/
EFI_STATUS
GsxRead(
   UINT32                *Data,
   IN EFI_PEI_SERVICES  **PeiServices
)
{
  return EFI_UNSUPPORTED;
}

/**

    GC_TODO: add routine description

    @param Data        - GC_TODO: add arg description
    @param PeiServices - GC_TODO: add arg description

    @retval None

**/
VOID
GetGsxBoardID(
   BOARD_ID             *Data,
   IN EFI_PEI_SERVICES  **PeiServices
)
{

  EFI_STATUS                Status;
  UINT32                    GSXIN[2];
  UINT32                    RetryCount;

  RetryCount = 0;
  GSXIN[0]   = 0;
  GSXIN[1]   = 0;

  do {
    Status     = GsxRead(GSXIN, PeiServices);
#ifdef PPO_FLAG
    //
    // ***PURLEY_PPO***  Hard coding to read in Platform ID zero (Neon City (for Mayan City))
    //
    GSXIN[0]	 = 0;
    GSXIN[1]	 = 0;
#endif

    if(Status){
      // if EFI_SUCCESS != Success then retry one more time
      RetryCount ++;
    }else{
      // if EFI_SUCCESS read Board ID and exit
      RetryCount = 0xFFFFFFFF;
    }

    if (GSXIN[0] & BIT0)
      Data->BoardID.BoardID0 = 1;

    if (GSXIN[0] & BIT1)
      Data->BoardID.BoardID1 = 1;

    if (GSXIN[0] & BIT2)
      Data->BoardID.BoardID2 = 1;

    if (GSXIN[0] & BIT3)
      Data->BoardID.BoardID3 = 1;

    if (GSXIN[0] & BIT4)
      Data->BoardID.BoardID4 = 1;

    if (GSXIN[0] & BIT5)
      Data->BoardID.BoardRev0 = 1;

    if (GSXIN[0] & BIT6)
      Data->BoardID.BoardRev1 = 1;

  } while(RetryCount < 1);

  if(Status){
    //
    // Unhable to read GSX HW error Hang the system
    //
    DEBUG ((EFI_D_ERROR, "ERROR: GSX HW is unavailable, SYSTEM HANG\n"));
    CpuDeadLoop();
  }
}

/**
    Get Platform Type by read Platform Data Region in SPI flash.
    SPI Descriptor Mode Routines for Accessing Platform Info from Platform Data Region (PDR)

    @param PeiServices  -  General purpose services available to every PEIM.
    @param PlatformInfoHob - Platform Type is returned in PlatformInfoHob->BoardId

    @retval Status EFI_SUCCESS - PDR read success
    @retval Status EFI_INCOMPATIBLE_VERSION - PDR read but it is not valid Platform Type

**/
EFI_STATUS
PdrGetPlatformInfo (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT EFI_PLATFORM_INFO         *PlatformInfoHob
  )
{
  EFI_STATUS              Status;
  EFI_SPI_PROTOCOL        *SpiPpi;
  UINTN                   Size;

  //
  // Locate the SPI PPI Interface
  //
  Status = (*PeiServices)->LocatePpi (
                    PeiServices,
                    &gPeiSpiPpiGuid,
                    0,
                    NULL,
                    &SpiPpi
                    );

  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Read the PIT (Platform Info Table) from the SPI Flash Platform Data Region
  //
  Size = sizeof (EFI_PLATFORM_INFO);
  Status = SpiPpi->FlashRead (
                              SpiPpi,
                              FlashRegionPlatformData,
                              PDR_REGION_START_OFFSET,
                              (UINT32) Size,
                              (UINT8 *) PlatformInfoHob
                             );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  if ((PlatformInfoHob->BoardId >= TypePlatformMin) && (PlatformInfoHob->BoardId <= TypePlatformMax)) {
    //
    // Valid Platform Identified
    //
    DEBUG ((EFI_D_ERROR, "Platform Info from PDR: Type = %x\n",PlatformInfoHob->BoardId));
  } else {
    //
    // Reading PIT from SPI PDR Failed or a unknown platform identified
    //
    DEBUG ((EFI_D_ERROR, "PIT from SPI PDR reports Platform ID as %x. This is unknown ID. Assuming Greencity Platform!\n", PlatformInfoHob->BoardId));
    PlatformInfoHob->BoardId = TypePlatformUnknown;
    Status = EFI_INCOMPATIBLE_VERSION;
  }
  return Status;
}

VOID
GpioConfigForBoardId(
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

VOID
GpioConfigForBoardRevId (VOID)
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

  for (i=0;i<2;i++) {
    // Calculate PAD configuration offset
    PadCfgReg = 0x8 * (V_GPIO_PAD12 + i) + R_PCH_PCR_GPIO_GPP_C_PADCFG_OFFSET;

    // Write PADCFG DW0 register
    MmioAndThenOr32 (
      (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM1, PadCfgReg),
      ~(UINT32)Dw0RegMask,
      (UINT32)Dw0Reg
    );

    // Write PADCFG DW1 register
    MmioAndThenOr32 (
      (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM1, PadCfgReg + 0x4),
      ~(UINT32)Dw1RegMask,
      (UINT32)Dw1Reg
    );
  }
}


EFI_STATUS
GPIOGetPlatformInfo (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT EFI_PLATFORM_INFO   *PlatformInfoHob
  )
/**

    GC_TODO: add routine description

    @param PeiServices     - GC_TODO: add arg description
    @param PlatformInfoHob - GC_TODO: add arg description

    @retval EFI_UNSUPPORTED - GC_TODO: add retval description
    @retval EFI_SUCCESS     - GC_TODO: add retval description

**/
{

#if SMCPKG_SUPPORT == 0
  UINT32                  BoardId;
  BOARD_ID                Board_ID;
  UINT32                  BoardRev;

  EFI_PEI_PCI_CFG2_PPI    *PciCfgPpi;
  EFI_STATUS              Status;
  UINT32                  Data32;

  PciCfgPpi = (**PeiServices).PciCfg;
  ASSERT (PciCfgPpi != NULL);

  Board_ID.BoardID.BoardID0 = 0;
  Board_ID.BoardID.BoardID1 = 0;
  Board_ID.BoardID.BoardID2 = 0;
  Board_ID.BoardID.BoardID3 = 0;
  Board_ID.BoardID.BoardID4 = 0;
  Board_ID.BoardID.BoardRev0 = 0;
  Board_ID.BoardID.BoardRev1 = 0;

  GpioConfigForBoardId();
  PlatformInfoHob->BoardId = TypeNeonCityEPRP;
  DEBUG ((EFI_D_ERROR, "Use GPIO to read Board ID\n"));

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
  GpioConfigForBoardRevId();
  Status = GpioGetInputValue (GPIO_SKL_H_GPP_C12, &Data32);
  if(EFI_ERROR(Status))
   return Status;
  if (Data32)
    Board_ID.BoardID.BoardRev0 = 1;

  Status = GpioGetInputValue (GPIO_SKL_H_GPP_C13, &Data32);
  if(EFI_ERROR(Status))
   return Status;
  if (Data32)
    Board_ID.BoardID.BoardRev1 = 1;

    BoardRev = (UINT8)( Board_ID.BoardID.BoardRev0 +
                       (Board_ID.BoardID.BoardRev1 << 1)
                      );
	PlatformInfoHob->TypeRevisionId = BoardRev;

#if MAX_SOCKET > 4
  // Set TypeLightningRidgeEX8S1N
  BoardId = 0x16;
#endif

  switch (BoardId) {
    case 0x0B:
      PlatformInfoHob->BoardId = TypeNeonCityEPRP;
      Status = PcdSet16S (PcdOemSkuSubBoardID, TypeBarkPeak);
      ASSERT_EFI_ERROR(Status);
      if (EFI_ERROR(Status)) return Status;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeBarkPeak\n"));
      break;
    case 0x00:
      PlatformInfoHob->BoardId = TypeNeonCityEPRP;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeNeonCityEPRP\n"));
      break;
    case 0x01:
      PlatformInfoHob->BoardId = TypeWolfPass;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeWolfPass\n"));
      break;
    case 0x02:
      PlatformInfoHob->BoardId = TypeNeonCityEPECB;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeNeonCityEPECB\n"));
      break;
    case 0x03:
      PlatformInfoHob->BoardId = TypePurleyLBGEPDVP;
      DEBUG ((EFI_D_ERROR, "Board ID = TypePurleyLBGEPDVP\n"));
      break;
    case 0x06:
      PlatformInfoHob->BoardId = TypeOpalCitySTHI;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeOpalCitySTHIPPV\n"));
      // Save BoardId Subtype for later usage
      Status = PcdSet16S (PcdOemSkuSubBoardID, TypePlatformOpalCityPPV);
      ASSERT_EFI_ERROR(Status);
      if (EFI_ERROR(Status)) return Status;
      break;
    case 0x07:
      PlatformInfoHob->BoardId = TypeOpalCitySTHI;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeOpalCitySTHICPV\n"));
      // Save BoardId Subtype for later usage
      Status = PcdSet16S (PcdOemSkuSubBoardID, TypePlatformOpalCityCPV);
      ASSERT_EFI_ERROR(Status);
      if (EFI_ERROR(Status)) return Status;
      break;
    case 0x08:
      PlatformInfoHob->BoardId = TypeBuchananPass;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeBuchananPass\n"));
      break;
    case 0x09:
      PlatformInfoHob->BoardId = TypeCrescentCity;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeCrescentCity\n"));
      break;
    case 0x0C:
      PlatformInfoHob->BoardId = TypeNeonCityFPGA;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeNeonCityFPGA\n"));
      break;
    case 0x0E:
      PlatformInfoHob->BoardId = TypeOpalCityFPGA;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeOpalCityFPGA\n"));
      break;
    case 0x19:
      PlatformInfoHob->BoardId = TypeHedtCRB;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeHedtCRB\n"));
      break;
    case 0x1A:
      PlatformInfoHob->BoardId = TypeHedtEV;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeHedtEV\n"));
      break;
    case 0x10:
      PlatformInfoHob->BoardId = TypeLightningRidgeEXRP;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeLightningRidgeEXRP\n"));
      break;
    case 0x12:
      PlatformInfoHob->BoardId = TypeLightningRidgeEXECB1;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeLightningRidgeEXECB1\n"));
      break;
    case 0x13:
      PlatformInfoHob->BoardId = TypeLightningRidgeEXECB2;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeLightningRidgeEXECB2\n"));
      break;
    case 0x14:
      PlatformInfoHob->BoardId = TypeLightningRidgeEXECB3;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeLightningRidgeEXECB3\n"));
      break;
    case 0x15:
      PlatformInfoHob->BoardId = TypeLightningRidgeEXECB4;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeLightningRidgeEXECB4\n"));
      break;
    case 0x16:
      PlatformInfoHob->BoardId = TypeLightningRidgeEX8S1N;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeLightningRidgeEX8S1N\n"));
      break;
    case 0x17:
      PlatformInfoHob->BoardId = TypeLightningRidgeEX8S2N;
      DEBUG ((EFI_D_ERROR, "Board ID = TypeLightningRidgeEX8S2N\n"));
      break;
  case 0x1F:
    PlatformInfoHob->BoardId = TypeKyanite;
    DEBUG((EFI_D_ERROR, "Board ID = TypeKyanite\n"));
    break;
    default:
      PlatformInfoHob->BoardId = TypePlatformDefault;
      DEBUG ((EFI_D_ERROR, "Board ID = %2X Default set to TypePlatformDefault\n",BoardId));
      break;

  }
  DEBUG ((EFI_D_ERROR, "Board Rev.: %d\n", BoardRev));
#else
  PlatformInfoHob->TypeRevisionId = 0;
  PlatformInfoHob->BoardId = SmcBoardType;
#endif
  return EFI_SUCCESS;
}

/**

  This function initializes the board related flag to indicates if
  PCH and Lan-On-Motherboard (LOM) devices is supported.

**/
VOID
GetPchLanSupportInfo(
  IN EFI_PLATFORM_INFO   *PlatformInfoHob
  )
{
  PlatformInfoHob->PchData.LomLanSupported  = 0;
}

/**

    GC_TODO: add routine description

    @param PeiVariable     - GC_TODO: add arg description
    @param PlatformInfoHob - GC_TODO: add arg description

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
EFI_STATUS
EFIAPI
GetIioCommonRcPlatformSetupPolicy(
  OUT EFI_PLATFORM_INFO                     *PlatformInfoHob
  )
  {
  EFI_STATUS                            Status;
  UINT8                                 IsocEn;

  Status = GetOptionData (&gEfiSocketCommonRcVariableGuid, OFFSET_OF (SOCKET_COMMONRC_CONFIGURATION, IsocEn), &IsocEn, sizeof(UINT8));

  if (EFI_ERROR (Status)) {
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
    PlatformInfoHob->SysData.IsocEn = 1;
#else
    PlatformInfoHob->SysData.IsocEn = 0;
#endif // ME_SUPPORT_FLAG
  } else {
    PlatformInfoHob->SysData.IsocEn    = IsocEn;       // ISOC enabled
  }

  return EFI_SUCCESS;
}
/**

    GC_TODO: add routine description

    @param PeiVariable     - GC_TODO: add arg description
    @param PlatformInfoHob - GC_TODO: add arg description

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
EFI_STATUS
EFIAPI
GetIioPlatformSetupPolicy(
  OUT EFI_PLATFORM_INFO                     *PlatformInfoHob
  )
{
  return EFI_SUCCESS;
}


/**
  Platform Type detection. Because the PEI globle variable
  is in the flash, it could not change directly.So use
  2 PPIs to distinguish the platform type.

  @param FfsHeader    -  Pointer to Firmware File System file header.
  @param PeiServices  -  General purpose services available to every PEIM.

  @retval EFI_SUCCESS  -  Memory initialization completed successfully.
  @retval Others       -  All other error conditions encountered result in an ASSERT.

**/
EFI_STATUS
EFIAPI
PlatformInfoInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS              Status;
  EFI_PEI_PCI_CFG2_PPI    *PciCfgPpi;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *PeiVariable;
  EFI_PLATFORM_INFO       PlatformInfoHob;
  EFI_PLATFORM_INFO       tempPlatformInfoHob;
  UINT8                   ChipId;
  UINT32                  Delay;
  UINT32                  CpuType;
  UINT8                   CpuStepping;
  UINT16                  Data16;
  UINT8                   Data8;

  //
  // Report Status Code EFI_CU_CHIPSET_PLATFORM_TYPE_INIT
  //
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    (EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_CHIPSET_PLATFORM_TYPE_INIT)
    );

  PciCfgPpi = (**PeiServices).PciCfg;
  if (PciCfgPpi == NULL) {
    DEBUG ((EFI_D_ERROR, "\nError! PlatformInfoInit() - PeiServices is a NULL Pointer!!!\n"));
    ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
    return EFI_INVALID_PARAMETER;
  }

   //
  // Locate Variable PPI
  //
  Status = (**PeiServices).LocatePpi (
                  PeiServices,
                  &gEfiPeiReadOnlyVariable2PpiGuid,
                  0,
                  NULL,
                  &PeiVariable
                  );

  (*PeiServices)->SetMem (
                    &PlatformInfoHob,
                    sizeof (PlatformInfoHob),
                    0
                    );

  //
  // --------------------------------------------------
  //
  // Detect the iBMC SIO for CV/CRB Platforms
  // 0x2E/0x2F decoding has been enabled in MonoStatusCode PEIM.
  //
  IoWrite8 (PILOTIV_SIO_INDEX_PORT, PILOTIV_SIO_UNLOCK);
  for (Delay = 0; Delay < 40; Delay++) IoRead8  (0x61);
  IoWrite8 (PILOTIV_SIO_INDEX_PORT, PILOTIV_CHIP_ID_REG);
  for (Delay = 0; Delay < 40; Delay++) IoRead8  (0x61);
  ChipId = IoRead8  (PILOTIV_SIO_DATA_PORT);
  for (Delay = 0; Delay < 40; Delay++) IoRead8  (0x61);
  IoWrite8 (PILOTIV_SIO_INDEX_PORT, PILOTIV_SIO_LOCK);
  for (Delay = 0; Delay < 40; Delay++) IoRead8  (0x61);

  if (EFI_ERROR (Status))
  {
        DEBUG((EFI_D_ERROR, "LocatePpi Error in PlatformInfo.c !\n"));
  }

  GetIioPlatformSetupPolicy(&PlatformInfoHob);
  GetIioCommonRcPlatformSetupPolicy(&PlatformInfoHob);

  //
  // Update PCH Type
  //
  //
  // Device ID
  //
  PciCfgPpi->Read (
              PeiServices,
              PciCfgPpi,
              EfiPeiPciCfgWidthUint16,
              PEI_PCI_CFG_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, PCI_DEVICE_ID_OFFSET),
              &Data16
              );
  PlatformInfoHob.PchSku= Data16;

  PlatformInfoHob.PchType = 0; // SuperSKU

  PciCfgPpi->Read (
              PeiServices,
              PciCfgPpi,
              EfiPeiPciCfgWidthUint8,
              PEI_PCI_CFG_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, PCI_REVISION_ID_OFFSET),
              &Data8
              );
  PlatformInfoHob.PchRevision = Data8;

  Status = EFI_SUCCESS;

    if(!EFI_ERROR(Status)) {
      Status = GPIOGetPlatformInfo(PeiServices, &PlatformInfoHob);
      if(EFI_ERROR(Status)) {
        Status = PdrGetPlatformInfo(PeiServices, &tempPlatformInfoHob);
        PlatformInfoHob.BoardId = tempPlatformInfoHob.BoardId;
        PlatformInfoHob.TypeRevisionId = tempPlatformInfoHob.TypeRevisionId;
        if (EFI_ERROR(Status)) {
          PlatformInfoHob.BoardId = TypePlatformUnknown;
        }
      }
    } else {
      PlatformInfoHob.BoardId = TypePlatformUnknown;
    }

  //
  // Update IIO Type
  //
  PlatformInfoHob.IioRevision = 0;


  //
  // Get Subtractive decode enable bit from descriptor
  //

  if (PchIsGbeRegionValid () == FALSE) {
    PlatformInfoHob.PchData.GbeRegionInvalid = 1;
  } else {
    PlatformInfoHob.PchData.GbeRegionInvalid = 0;
  }
  GetPchLanSupportInfo(&PlatformInfoHob);
  PlatformInfoHob.PchData.GbePciePortNum = 0xFF;
  PlatformInfoHob.PchData.GbePciePortNum = (UINT8) PchGetGbePortNumber ();
  PlatformInfoHob.PchData.GbeEnabled  = PchIsGbePresent ();
  PlatformInfoHob.PchData.PchStepping = PchStepping ();
  PlatformInfoHob.SysData.SysSioExist = (UINT8)IsSioExist();

  GetCpuInfo (&CpuType, &CpuStepping);
  PlatformInfoHob.CpuType     = CpuType;
  PlatformInfoHob.CpuStepping = CpuStepping;
  // APTIOV_SERVER_OVERRIDE_RC_START : Oem Hook to update PlatformInfo HOB
  OemUpdatePlatformInfoHob (PeiServices, (VOID *) &PlatformInfoHob);
  // APTIOV_SERVER_OVERRIDE_RC_END : Oem Hook to update PlatformInfo HOB

  //
  // Build HOB for setup memory information
  //

  BuildGuidDataHob (
      &gEfiPlatformInfoGuid,
      &(PlatformInfoHob),
      sizeof (EFI_PLATFORM_INFO)
      );

  Status = (**PeiServices).InstallPpi (PeiServices, &mPlatformInfoPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Save PlatformInfoHob.BoardId in CMOS
  //
  IoWrite8 (R_IOPORT_CMOS_UPPER_INDEX, CMOS_PLATFORM_ID_LO);
  IoWrite8 (R_IOPORT_CMOS_UPPER_DATA, (UINT8)PlatformInfoHob.BoardId);

  IoWrite8 (R_IOPORT_CMOS_UPPER_INDEX, CMOS_PLATFORM_ID_HI);
  IoWrite8 (R_IOPORT_CMOS_UPPER_DATA, (UINT8)((PlatformInfoHob.PcieRiser2Type << 4) + (PlatformInfoHob.PcieRiser1Type)));

  return EFI_SUCCESS;
}
