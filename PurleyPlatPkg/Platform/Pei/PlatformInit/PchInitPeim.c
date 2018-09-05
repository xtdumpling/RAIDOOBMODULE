//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.02
//    Bug Fix:  Add debug message through SOL function
//    Reason:   
//    Auditor:  Jacker Yeh
//    Date:     Feb/08/2017
//
//  Rev. 1.01
//    Bug Fix:  Fixed chassis intrusion status in Sensor Readings of IPMI web will be reset after AC on.
//    Reason:   
//    Auditor:  Jimmy Chiu(Refer Grantley - Chen Lin)
//    Date:     Aug/19/2016
//
//  Rev. 1.00
//    Bug Fix:  Fix S-SATA4 port can't detect HDD.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jun/02/2016
//
//***************************************************************************
/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c)  2012-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file

    PchInitPeim.c

  @brief
    Do Early PCH initialization.

**/
#include <Token.h>	//SMCPKG_SUPPORT
#include "PlatformEarlyInit.h"

#include <Ppi/PchReset.h>
#include <Library/PchPcrLib.h>
#include <Library/PchInfoLib.h>
// APTIOV_SERVER_OVERRIDE_RC_START : Use OEM functions from AmiPlatformHooksLib.h
#include <Library/AmiPlatformHooksLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Use OEM functions from AmiPlatformHooksLib.h
#ifdef PC_HOOK
#include <Library/GpioLib.h>
#define   TypeWolfPassQAT					60
#define   TypeBuchananPassQAT				61
#define   TypeSawtoothPassQAT				62
#define   TypeBuchananPassSFP				63
#define QAT_ENABLED                0  // QAT is active-low
#endif

/**
  isFlashDescriptorOverrideDisadled - DFX function. Checks if validation team allows for Flash Descriptor override

  @param[in] PeiServices            Pointer to the PeiServices
  @param[in] SpiProtocol            Pointer to the SpiProtocol

  @retval TRUE                           Flash descriptor is locked for write
  @retval FALSE                          Flash descriptor is open for write
**/
BOOLEAN
isFlashDescriptorOverrideDisadled(
  IN EFI_PEI_SERVICES **PeiServices,
  IN EFI_SPI_PROTOCOL *SpiProtocol,
  IN UINT32            fmba
  )
{
  SPI_REGION_SV      SpiRegionSV;
  EFI_STATUS         Status;
  FLASH_REGION_TYPE  FlashRegion;
  UINT32             flmap0;
  UINT32             frba;
  UINT32             sv_frba;
  UINT32             Data32;
  UINT32             sv_permission_bits = 0;

  DEBUG((EFI_D_INFO, "isFlashDescriptorOverrideDisadled() Entry \n"));
  FlashRegion = FlashRegion14;

  //
  // Check if SV region is avaliable
  //

  // Read Flash MAP 0
  Status = SpiProtocol->FlashRead(SpiProtocol, FlashRegionDescriptor, SPI_DESCR_ADDR_FLMAP0, 4, (UINT8 *)&flmap0);
  ASSERT_EFI_ERROR (Status);

  // Extract Flash Region Base Address
  frba = 0;
  frba |= (flmap0 & 0x00FF0000) >> 12;

  // Flash Region Base Address for SV Region
  Status = SpiProtocol->FlashRead(SpiProtocol, FlashRegionDescriptor, (frba + (FlashRegion * 4)), 4, (UINT8 *)&sv_frba);
  ASSERT_EFI_ERROR (Status);
  DEBUG((EFI_D_INFO, "Flash MAP 0= 0x%08x, FRBA= 0x%08x, SV Region FRBA= 0x%08x\n", flmap0, frba, sv_frba));
  if (sv_frba == 0x00007FFF) {
    DEBUG((EFI_D_INFO, "Flash Region %d is disabled.\n", FlashRegion));
    return FALSE;
  }

  //
  // Check if Host CPU/BIOS has access to SV region
  //

  //Flash Master 1
  Status = SpiProtocol->FlashRead(SpiProtocol, FlashRegionDescriptor, fmba, 4, (UINT8 *)&Data32);
  ASSERT_EFI_ERROR (Status);

  if (FlashRegion < 8) {
    sv_permission_bits |= (1 << (FlashRegion+20)) | (1 << (FlashRegion+8));
  } else {
    sv_permission_bits |= (1 << (FlashRegion-8)) | (1 << (FlashRegion-12));
  }
  if ((Data32 & sv_permission_bits) != sv_permission_bits) {
    DEBUG((EFI_D_INFO, "isFlashDescriptorOverrideDisadled(): Host CPU/BIOS Master Read/Write Access for SV region DISABLED!\n"));
    DEBUG((EFI_D_INFO, "isFlashDescriptorOverrideDisadled(): Assume descriptor override is enabled\n"));
    return FALSE;
  }

  //
  // Check Descriptor Override Flag
  //

  // Read 16 bytes from SV region, offset 0
  Status = SpiProtocol->FlashRead(SpiProtocol, FlashRegion, 0, sizeof(SPI_REGION_SV), (UINT8 *)&SpiRegionSV);
  ASSERT_EFI_ERROR (Status);

  if (SpiRegionSV.signature == SPI_REGION_SV_SIGNATURE) {
    if ((SpiRegionSV.sv_flags & SPI_REGION_SV_FLAG_DESC_OVERR_DIS) == SPI_REGION_SV_FLAG_DESC_OVERR_DIS) {
      DEBUG((EFI_D_INFO, "isFlashDescriptorOverrideDisadled(): Descriptor Override is DISABLED by SV\n"));
      return TRUE;
    }
  } else {
    DEBUG((EFI_D_INFO, "Error: SV region signature not match! (0x%08x != 0x%08x)\n", SpiRegionSV.signature, SPI_REGION_SV_SIGNATURE));
  }

  DEBUG((EFI_D_INFO, "isFlashDescriptorOverrideDisadled() End \n"));
  return FALSE;
}

/**
  isFlashDescriptorLocked -         Checks Flash Descriptor access for write

  @param[in] PeiServices            Pointer to the PeiServices

  @retval TRUE                           Flash descriptor is locked for write
  @retval FALSE                          Flash descriptor is open for write
**/
BOOLEAN
isFlashDescriptorLocked(
  IN EFI_PEI_SERVICES **PeiServices
  )
{
  EFI_SPI_PROTOCOL   *SpiProtocol = NULL;
  UINT32             Flmap1;
  UINT32             fmba;
  UINT32             Data32;
  EFI_STATUS         Status;

  DEBUG((EFI_D_INFO, "isFlashDescriptorLocked() Entry \n"));

  Status = (**PeiServices).LocatePpi(
                              PeiServices,
                              &gPeiSpiPpiGuid,
                              0,
                              NULL,
                              &SpiProtocol
                              );
  ASSERT_EFI_ERROR (Status);

  //
  // Check Host CPU/BIOS Master Region Write Access for Descriptor region
  //
  Status = SpiProtocol->FlashRead(SpiProtocol, FlashRegionDescriptor, SPI_DESCR_ADDR_FLVALSIG, 4, (UINT8 *)&Data32);
  ASSERT_EFI_ERROR (Status);
  if(Data32 != V_PCH_SPI_FDBAR_FLVALSIG) {
    //Valid signature NOT found
    DEBUG((EFI_D_ERROR, "Error! Valid flash signature NOT found!\n"));
    return TRUE;
  }
  // Read Flash MAP 1
  Status = SpiProtocol->FlashRead(SpiProtocol, FlashRegionDescriptor, SPI_DESCR_ADDR_FLMAP1, 4, (UINT8 *)&Flmap1);
  ASSERT_EFI_ERROR (Status);

  // Extract Flash Master Base Address
  fmba = 0;
  fmba |= (Flmap1 & 0x000000FF) << 4;

  //Flash Master 1 - Host CPU / BIOS
  Status = SpiProtocol->FlashRead(SpiProtocol, FlashRegionDescriptor, fmba, 4, (UINT8 *)&Data32);
  ASSERT_EFI_ERROR (Status);
  DEBUG((EFI_D_INFO, "Flash MAP 1= 0x%08x, FMBA= 0x%08x, Flash Master 1= 0x%08x\n", Flmap1, fmba, Data32));

  if ((Data32 & BIT20) != BIT20) {
    // Host CPU/BIOS Master Region Write Access for Descriptor region DISABLED
    DEBUG((EFI_D_INFO, "isFlashDescriptorLocked(): Host CPU/BIOS Master Write Access for descriptor region DISABLED\n"));
    return TRUE;
  }
  // Check if softstrap override is disabled by SV
  if (isFlashDescriptorOverrideDisadled(PeiServices, SpiProtocol, fmba) == TRUE) {
    return TRUE;
  }
  DEBUG((EFI_D_INFO, "isFlashDescriptorLocked() End \n"));
  return FALSE;
}

//
// Function necessary for:
// WA: PPO NeonCity PCIe/SATA port selection- 4929750: Sata express  and M.2 sata or pcie selection gpio is on P0 instead of P2, and P5 instead of P4
//
VOID
GpioConfigForPcieSataPortSelect(
  VOID
  )
{
  UINT32               Dw0Reg = 0;
  UINT32               Dw0RegMask = 0;
  UINT32               PadCfgReg;

  Dw0RegMask |= ((V_PCH_GPIO_RST_CONF_DEEP_RST << N_PCH_GPIO_RST_CONF) | B_PCH_GPIO_TXDIS | B_PCH_GPIO_PAD_MODE);
  Dw0Reg |= ((V_PCH_GPIO_RST_CONF_DEEP_RST << N_PCH_GPIO_RST_CONF) | B_PCH_GPIO_TXDIS | (V_PCH_GPIO_PAD_MODE_GPIO << N_PCH_GPIO_PAD_MODE));

  // Calculate PAD configuration offset for GPIO_SKL_H_GPP_H23
  PadCfgReg = 0x8 * (V_GPIO_PAD23) + R_PCH_H_PCR_GPIO_GPP_H_PADCFG_OFFSET;
  // Write PADCFG DW0 register  for GPIO H23
  MmioAndThenOr32 (
    (UINTN)PCH_PCR_ADDRESS (PID_GPIOCOM5, PadCfgReg),
    (UINT32)Dw0RegMask,
    (UINT32)Dw0Reg
  );
}

/**
  PchSoftStrapFixup - Overrides PCH.softstraps based on Board ID

  @param[in] PeiServices                 Pointer to the PeiServices
  @param[in] SystemBoard              Pointer to the SystemBoard PPI

  @retval EFI_SUCCESS                   PCH Softstraps updated (reset is required)
  @retval EFI_ABORTED                   PCH Softstrap update not needed for this platform or flash desctiptor is NOT writable
  @retval EFI_INVALID_PARAMETER  Unknown platform
**/
EFI_STATUS
EFIAPI
PchSoftStrapFixup (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN SYSTEM_BOARD_PPI            *SystemBoard
  )
{
  UINT32                      CurrentStrapValue;
  UINT32                      NewStrapValue;
  const SOFTSTRAP_FIXUP_ENTRY *SoftstrapFixupEntry;
  EFI_SPI_PROTOCOL            *SpiProtocol = NULL;
  EFI_STATUS                  Status;
  UINT32                      StrapAddr = 0;
  UINT64                      Mask;
  UINTN                       Count;
  UINT32                      Flmap1;
  UINT8                       *BackUpBlock;
  UINT8                       *NewBlock;
  UINT32                      StrapBaseAddress;
  UINT32                      BackUpSize;
  UINT8                       *StrapRegion;
#if SMCPKG_SUPPORT == 0 // Supermicro
  UINT32                      Data32;
#endif // #if SMCPKG_SUPPORT == 0 // Supermicro
  UINT8                       BoardId = SystemBoard->SystemBoardIdValue();
#ifdef PC_HOOK
  UINT8						  QATId=0xff;
  UINT32     				  QATGpio;
  UINT32     				  BoardID0Gpio;


  {
    GPIO_CONFIG             			GpioData;
    GpioData.PadMode = GpioPadModeGpio;
    GpioData.HostSoftPadOwn = GpioHostOwnDefault;
    GpioData.Direction = GpioDirIn;
    GpioData.OutputState = GpioOutDefault;
    GpioData.InterruptConfig = GpioIntDefault;
    GpioData.PowerConfig = GpioResetDefault;
    GpioData.ElectricalConfig = GpioTermDefault;
    GpioData.LockConfig = GpioLockDefault;
    GpioData.OtherSettings = GpioRxRaw1Default;
    GpioSetPadConfig (GPIO_SKL_H_GPP_B3, &GpioData);
  }
  Status = GpioGetInputValue (GPIO_SKL_H_GPP_B3,  &QATGpio);

  {
	  GPIO_CONFIG             			GpioData;
	  GpioData.PadMode = GpioPadModeGpio;
	  GpioData.HostSoftPadOwn = GpioHostOwnDefault;
	  GpioData.Direction = GpioDirIn;
	  GpioData.OutputState = GpioOutDefault;
	  GpioData.InterruptConfig = GpioIntDis;
	  GpioData.PowerConfig = GpioResetNormal;
	  GpioData.ElectricalConfig = GpioTermNone;
	  GpioData.LockConfig = GpioLockDefault;
	  GpioData.OtherSettings = GpioRxRaw1Default;
	  GpioSetPadConfig(GPIO_SKL_H_GPP_G12, &GpioData);
  }
  Status = GpioGetInputValue(GPIO_SKL_H_GPP_G12, &BoardID0Gpio);
  DEBUG((DEBUG_ERROR, "[Alex]: BoardID0Gpio =%x \n", BoardID0Gpio));

#endif  

  DEBUG ((DEBUG_INFO, "PchSoftStrapFixup() - Start\n"));

  // APTIOV_SERVER_OVERRIDE_RC_START : Do not fixup Straps for Simics
  if (IsSimicsPlatform() == TRUE) {
  	return EFI_ABORTED;
  }
  // APTIOV_SERVER_OVERRIDE_RC_END : Do not fixup Straps for Simics
  

  if (isFlashDescriptorLocked(PeiServices) == TRUE) {
    return EFI_ABORTED;
  }

  if (SystemBoard->SystemBoardIdSkuValue() == TypeBarkPeak) {
    BoardId = TypePurleyLBGEPDVP;
  }

  if (BoardId > TypePlatformMax) {
    DEBUG((DEBUG_ERROR,"PchFixupSofStraps: Platform not identified.\n"));
    return EFI_INVALID_PARAMETER;
  }

  Status = (**PeiServices).LocatePpi(
                              PeiServices,
                              &gPeiSpiPpiGuid,
                              0,
                              NULL,
                              &SpiProtocol
                              );
  ASSERT_EFI_ERROR (Status);

  // Read Flash MAP 1
  Status = SpiProtocol->FlashRead(SpiProtocol, FlashRegionDescriptor, SPI_DESCR_ADDR_FLMAP1, 4, (UINT8 *)&Flmap1);
  ASSERT_EFI_ERROR (Status);

  // Extact Flash PCH Strap Base Address (FPSBA)
  StrapBaseAddress = (Flmap1 & 0x00FF0000) >> 12;

  BackUpSize = SIZE_4KB;
  Status   = (*PeiServices)->AllocatePool (
                           PeiServices,
                           BackUpSize,
                           &BackUpBlock
                           );
  if(Status!= EFI_SUCCESS){
    DEBUG((DEBUG_ERROR,  "PchSoftStrapFixup: ERROR: Unable to allocate buffer of size=%x \n", BackUpSize));
    return(Status);
  }

  Status   = (*PeiServices)->AllocatePool (
                           PeiServices,
                           BackUpSize,
                           &NewBlock
                           );
  if(Status!= EFI_SUCCESS){
    DEBUG((DEBUG_ERROR,  "PchSoftStrapFixup: ERROR: Unable to allocate buffer of size=%x \n", BackUpSize));
    return(Status);
  }

  Status = SpiProtocol->FlashRead(SpiProtocol, FlashRegionDescriptor, 0, BackUpSize, BackUpBlock);
  ASSERT_EFI_ERROR (Status);

  CopyMem((VOID *)NewBlock, (VOID *)BackUpBlock, BackUpSize);

  StrapRegion = NewBlock + StrapBaseAddress;
#ifdef PC_HOOK
//Check QAT SKU by QATGpio to update relative Soft strap

    if(QATGpio ==QAT_ENABLED){
		if(BoardId == TypeWolfPass){
		QATId=TypeWolfPassQAT;
		}
		if(BoardId == TypeBuchananPass){
		QATId=TypeBuchananPassQAT;
		}
		if(BoardId == TypeSawtoothPass){
		QATId=TypeSawtoothPassQAT;
		}
    }

	if (BoardId == TypeBuchananPass){
		if (BoardID0Gpio == 0x01){
			QATId = TypeBuchananPassSFP;  // BNP: "1" is SFP+, "0" is RJ45.
		}
	}
	DEBUG((DEBUG_ERROR, "QATId =%x \n", QATId));

 for (SoftstrapFixupEntry = SystemBoard->SystemBoardInfo->SoftstrapFixupTable; SoftstrapFixupEntry->BoardId != TypePlatformUnknown; SoftstrapFixupEntry++) {
    // check if BoardId is same as softstrap fixup table, and TypeLightningRidgeEXRP uses TypeLightningRidgeEXECB1 softstrap (they use same softstrap)


    if ((BoardId == SoftstrapFixupEntry->BoardId) || (QATId == SoftstrapFixupEntry->BoardId) || ((SoftstrapFixupEntry->BoardId  == TypeLightningRidgeEXECB1) && (BoardId == TypeLightningRidgeEXRP))) {

		DEBUG((DEBUG_ERROR, "ID:%x SoftStrapNumber:%x\n", SoftstrapFixupEntry->BoardId, SoftstrapFixupEntry->SoftStrapNumber));
//
// LBG stepping A0/B0 single SPI image WA section - Start
//
      if ((PchStepping() >= LbgB0)
          && SoftstrapFixupEntry->SoftStrapNumber <= 131 && SoftstrapFixupEntry->SoftStrapNumber >= 124)
        continue;
//
// LBG stepping A0/B0 single SPI image WA section - End
//
      StrapAddr = SoftstrapFixupEntry->SoftStrapNumber * sizeof(UINT32);
      NewStrapValue = CurrentStrapValue = *(UINT32 *)(StrapRegion + StrapAddr);
      Mask = LShiftU64((UINT64)1, SoftstrapFixupEntry->BitfieldOffset);
      Count = 1;
      while (Count++ < SoftstrapFixupEntry->BitfieldLength) {
        Mask |= MultU64x32(Mask, 2);
      }
      NewStrapValue &= ~Mask;
      NewStrapValue |= (SoftstrapFixupEntry->Value << SoftstrapFixupEntry->BitfieldOffset) & Mask;
      *(UINT32 *)(StrapRegion + StrapAddr) = NewStrapValue;
    }
  }
#else
  for (SoftstrapFixupEntry = SystemBoard->SystemBoardInfo->SoftstrapFixupTable; SoftstrapFixupEntry->BoardId != TypePlatformUnknown; SoftstrapFixupEntry++) {
    // check if BoardId is same as softstrap fixup table
    if (BoardId == SoftstrapFixupEntry->BoardId) {

//
// LBG stepping A0/B0 single SPI image WA section - Start
//
      if ((PchStepping() >= LbgB0)
          && SoftstrapFixupEntry->SoftStrapNumber <= 131 && SoftstrapFixupEntry->SoftStrapNumber >= 124)
        continue;
//
// LBG stepping A0/B0 single SPI image WA section - End
//
      StrapAddr = SoftstrapFixupEntry->SoftStrapNumber * sizeof(UINT32);
      NewStrapValue = CurrentStrapValue = *(UINT32 *)(StrapRegion + StrapAddr);
      Mask = LShiftU64((UINT64)1, SoftstrapFixupEntry->BitfieldOffset);
      Count = 1;
      while (Count++ < SoftstrapFixupEntry->BitfieldLength) {
        Mask |= MultU64x32(Mask, 2);
      }
      NewStrapValue &= ~Mask;
      NewStrapValue |= (SoftstrapFixupEntry->Value << SoftstrapFixupEntry->BitfieldOffset) & Mask;
      *(UINT32 *)(StrapRegion + StrapAddr) = NewStrapValue;
    }
  }
#endif
#if SMCPKG_SUPPORT == 0 // Supermicro

  //
  // WA: PPO NeonCity PCIe/SATA port selection- 4929750: Sata express  and M.2 sata or pcie selection gpio is on P0 instead of P2, and P5 instead of P4
  //
  if (SystemBoard->PcieSataPortSelectionWA() == TRUE) {
    GpioConfigForPcieSataPortSelect();
    GpioGetInputValue (GPIO_SKL_H_GPP_H23, &Data32);
    StrapAddr = 0x48;
    NewStrapValue = *(UINT32 *)(StrapRegion + StrapAddr);
    NewStrapValue &= ~(BIT2 | BIT3);
    // SS18_FIA1
    // - PCIE_SATA_P4_STRP
    if (Data32) {
      // PCIe select
      NewStrapValue |= BIT2;
    } else {
      // SATA select
    }
    *(UINT32 *)(StrapRegion + StrapAddr) = NewStrapValue;

    StrapAddr = 0x3C;
    NewStrapValue = *(UINT32 *)(StrapRegion + StrapAddr);
    NewStrapValue &= ~(BIT9 | BIT8);
    // SS15_SSATA3
    // - SATA_PCIE_Select_for_Port_4
    if (Data32) {
      // PCIe select
      NewStrapValue |= BIT8;
    } else {
      // SATA select
    }
    *(UINT32 *)(StrapRegion + StrapAddr) = NewStrapValue;

    StrapAddr = 0x40;
    NewStrapValue = *(UINT32 *)(StrapRegion + StrapAddr);
    NewStrapValue &= ~(BIT9 | BIT8);
    // SS16_GPCOM5
    // - PCIE_SATA_COMBO_PORT_4_Strap
    if (Data32) {
     // PCIe select
      NewStrapValue |= BIT8;
    } else {
      // SATA select
    }
    *(UINT32 *)(StrapRegion + StrapAddr) = NewStrapValue;
  }
#endif // #if SMCPKG_SUPPORT == 0 // Supermicro
// APTIOV_SERVER_OVERRIDE_RC_START : Oem Hook to Override PchSoftStrap
  OemPchSoftStrapFixupOverride(StrapRegion, (VOID *) SystemBoard);
// APTIOV_SERVER_OVERRIDE_RC_END : Oem Hook to Override PchSoftStrap

  DEBUG ((DEBUG_INFO, "PchSoftStrapFixup() - End\n"));
  if (CompareMem(NewBlock, BackUpBlock, BackUpSize)) {
    Status = SpiProtocol->FlashErase(SpiProtocol, FlashRegionDescriptor, 0, BackUpSize);
    ASSERT_EFI_ERROR (Status);
    Status = SpiProtocol->FlashWrite(SpiProtocol, FlashRegionDescriptor, 0, BackUpSize, NewBlock);
    ASSERT_EFI_ERROR (Status);
    return EFI_SUCCESS;
  } else {
    return EFI_ABORTED;
  }
}

/**
FpkSelector - Gbeb binary is used for Lightning Ridge platform; Update Gbea Region Descriptor Register with Gbeb Register value

@param[in] PeiServices                Pointer to the PeiServices
@param[in] SystemBoard                Pointer to the SystemBoard PPI

@retval None
**/

VOID
EFIAPI
FpkSelector (
IN EFI_PEI_SERVICES             **PeiServices,
IN SYSTEM_BOARD_PPI             *SystemBoard
) {
  EFI_SPI_PROTOCOL              *SpiProtocol = NULL;
  EFI_STATUS                    Status;
  UINT32                        flmap0;
  UINT8                         *Gbeb, *Gbea;
  UINT8                         *NewBlock;
  UINT32                        RegionBaseAddress;
  UINT32                        BackUpSize;
  UINT8                         BoardId;;
  
  //
  // Gbeb binary is used for Lightning Ridge platform; 
  //
  DEBUG((EFI_D_INFO, "FpkSelector - Start\n"));
 
  BoardId = SystemBoard->SystemBoardIdValue();
  if ((BoardId < TypeLightningRidgeEXRP) || (BoardId > TypeLightningRidgeEX8S2N)) {
    DEBUG((EFI_D_ERROR, "No LR system return BoardId=%d\n", BoardId));
    return;
  }

  //
  // Get SpiProtocol instance for Flash access routines 
  //
  Status = (**PeiServices).LocatePpi(
    PeiServices,
    &gPeiSpiPpiGuid,
    0,
    NULL,
    &SpiProtocol
    );
  if(EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "%r Error in locating Spi ppi \n", Status));
    return;
  }

  //
  // Read Flash MAP 0
  //
  Status = SpiProtocol->FlashRead(SpiProtocol, FlashRegionDescriptor, SPI_DESCR_ADDR_FLMAP0, 0x4, (UINT8 *)&flmap0);
  if(EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "%r Error while reading Flash region Descriptor\n", Status));
    return;
  }
  
  //
  // Extract Flash Region Base Address
  //
  RegionBaseAddress = 0;
  RegionBaseAddress |= (flmap0 & 0x00FF0000) >> 12;

  //
  // Read Gbea and Gbeb pointer from Region Descriptor register FLREG11(Gbea) and FLREG12(Gbeb)
  //
  Status = SpiProtocol->FlashRead(SpiProtocol, FlashRegionDescriptor, (RegionBaseAddress + (FlashRegion10Gbe_A * 4)), 4, (UINT8 *)&Gbea);
  Status = SpiProtocol->FlashRead(SpiProtocol, FlashRegionDescriptor, (RegionBaseAddress + (FlashRegion10Gbe_B * 4)), 4, (UINT8 *)&Gbeb);
  DEBUG((EFI_D_INFO, "Gbea (*((UINT32 *)&Gbea) = %x\n", *((UINT32 *)&Gbea)));
  DEBUG((EFI_D_INFO, "Gbeb (*((UINT32 *)&Gbeb) = %x\n", *((UINT32 *)&Gbeb)));

  if ( (*((UINT32 *)&Gbeb) == 0x00007FFF) ) {
    //
    //return if GBEB is disabled or Gbeb equals to Gbea\n"));
    //
    DEBUG((EFI_D_ERROR, "Gbeb is disabled (*((UINT32 *)&Gbeb) = %x\n", *((UINT32 *)&Gbeb)));
    return;
  }

  if ((*((UINT32 *)&Gbeb)) < (*((UINT32 *)&Gbea))) {
    DEBUG((EFI_D_ERROR, "Gbeb is already updated (*((UINT32 *)&Gbeb) = %x\n", *((UINT32 *)&Gbeb)));
    return;
  }
  //
  // Allocate 4K sized memory pool to perform 4K Block SPI update
  //
  BackUpSize = SIZE_4KB;
  Status = (*PeiServices)->AllocatePool(
    PeiServices,
    BackUpSize,
    &NewBlock
    );
  if (Status != EFI_SUCCESS){
    DEBUG((EFI_D_ERROR, "Allocate pool failed\n"));
    return;
  }

  //
  // Read First 4K of SPI region 
  //
  Status = SpiProtocol->FlashRead(SpiProtocol, FlashRegionDescriptor, 0, BackUpSize, NewBlock);
  if(EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "%r Error while reading Flash region Descriptor\n", Status));
    return;
  }

  //
  // Update Gbea with Gbeb pointer value in 4K block buffer
  //
  *((UINT32 *)(NewBlock + RegionBaseAddress + FlashRegion10Gbe_A * 4)) = *(UINT32 *)&Gbeb;
  *((UINT32 *)(NewBlock + RegionBaseAddress + FlashRegion10Gbe_B * 4)) = *(UINT32 *)&Gbea;
  //
  // Update SPI flash by erasing and writing 4K block
  //
  Status = SpiProtocol->FlashErase(SpiProtocol, FlashRegionDescriptor, 0, BackUpSize);
  if(EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "%r Error while Erasing Flash region Descriptor\n", Status));
    return;
  }

  Status = SpiProtocol->FlashWrite(SpiProtocol, FlashRegionDescriptor, 0, BackUpSize, NewBlock);
  if(EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "%r Error while writing Flash region Descriptor\n", Status));
    return;
  }
  
  DEBUG((EFI_D_INFO, "FpkSelector - End\n"));
}

VOID
EarlyPlatformPchInit (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN SYSTEM_CONFIGURATION        *SystemConfiguration,
  IN PCH_RC_CONFIGURATION        *PchRcConfiguration,
  IN SYSTEM_BOARD_PPI            *SystemBoard
  )
{
  UINT16                          Data16;
  UINT8                           Data8;
  UINTN                           LpcBaseAddress;
  UINT8                           TcoRebootHappened;
  UINTN                           PmcBaseAddress;
  UINTN                           SpiBaseAddress;
  UINTN                           P2sbBase;
  BOOLEAN                         ResetRequired = FALSE;
  EFI_STATUS                      Status;
  PCH_RESET_PPI                   *PchResetPpi = NULL;
#ifdef ASPEED_ENABLE
  UINT16                          LpcIoeOrg;
#endif

  // APTIOV_SERVER_OVERRIDE_RC_START : eSPI CS#1 configuration programming
  #if defined(PILOT4_LPC_ESPI_SLAVE_CONNECTION) && PILOT4_LPC_ESPI_SLAVE_CONNECTION
      UINT8 SlaveDevice = ESPI_SECONDARY_SLAVE;
  #else
      UINT8 SlaveDevice = LPC_ESPI_FIRST_SLAVE;
  #endif
  // APTIOV_SERVER_OVERRIDE_RC_END : eSPI CS#1 configuration programming

  DEBUG((EFI_D_ERROR, "EarlyPlatformPchInit - Start\n"));

  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );
  PmcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_PMC,
                     PCI_FUNCTION_NUMBER_PCH_PMC
                     );
  SpiBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_SPI,
                     PCI_FUNCTION_NUMBER_PCH_SPI
                     );

  //
  // Program bar
  //
  P2sbBase = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_P2SB,
               PCI_FUNCTION_NUMBER_PCH_P2SB
               );

  MmioWrite32 (P2sbBase + R_PCH_P2SB_SBREG_BAR, PCH_PCR_BASE_ADDRESS);
  MmioOr8 (P2sbBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);

  //
  // LPC I/O Configuration
  //
#if SMCPKG_SUPPORT && (DEBUG_FROM_SOL == 1)
  PchLpcIoDecodeRangesSet (
    (V_PCH_LPC_IOD_LPT_378  << N_PCH_LPC_IOD_LPT)  |
    (V_PCH_LPC_IOD_COMB_3E8 << N_PCH_LPC_IOD_COMB) |
    (V_PCH_LPC_IOD_COMA_2F8 << N_PCH_LPC_IOD_COMA)
    );
#else
  PchLpcIoDecodeRangesSet (
    (V_PCH_LPC_IOD_LPT_378  << N_PCH_LPC_IOD_LPT)  |
    (V_PCH_LPC_IOD_COMB_3E8 << N_PCH_LPC_IOD_COMB) |
    (V_PCH_LPC_IOD_COMA_3F8 << N_PCH_LPC_IOD_COMA)
    );
#endif
  PchLpcIoEnableDecodingSet (
    B_PCH_LPC_IOE_ME2  |
    B_PCH_LPC_IOE_SE   |
    B_PCH_LPC_IOE_ME1  |
    B_PCH_LPC_IOE_KE   |
    B_PCH_LPC_IOE_HGE  |
    B_PCH_LPC_IOE_LGE  |
    B_PCH_LPC_IOE_FDE  |
    B_PCH_LPC_IOE_PPE  |
    B_PCH_LPC_IOE_CBE  |
    B_PCH_LPC_IOE_CAE,
  // APTIOV_SERVER_OVERRIDE_RC_START : eSPI CS#1 configuration programming
    SlaveDevice
  // APTIOV_SERVER_OVERRIDE_RC_END : eSPI CS#1 configuration programming
    );
#ifdef ASPEED_ENABLE
  LpcIoeOrg = MmioRead16(LpcBaseAddress + R_PCH_LPC_IOE);
  DEBUG ((EFI_D_ERROR, "[IPMI_DEBUG]: LpcIoeOrg: 0x%x\n", LpcIoeOrg));
#endif
  //
  // Enable the upper 128-byte bank of RTC RAM
  //
  PchPcrAndThenOr32 (PID_RTC, R_PCH_PCR_RTC_CONF, (UINT32)~0, B_PCH_PCR_RTC_CONF_UCMOS_EN);

// APTIOV_SERVER_OVERRIDE_RC_START : BMC KCS base address decoding is done by IPMI module
#if 0  
#ifdef ASPEED_ENABLE
  //
  // Enable LPC decode at 0xCA2 for Pilot BMC, 0x600 for Mailbox
  //
  Status = PchLpcGenIoRangeSet ((IPMI_DEFAULT_SMM_IO_BASE  & 0xFF0), 0x10, LPC_ESPI_FIRST_SLAVE);
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "[IPMI_DEBUG]: PchLpcGenIoRangeSet CA0!\n"));
  }
  Status = PchLpcGenIoRangeSet ((PILOTIV_MAILBOX_BASE_ADDRESS  & 0xFF0), 0x10, LPC_ESPI_FIRST_SLAVE);
  if (Status == EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "[IPMI_DEBUG]: PchLpcGenIoRangeSet 600!\n"));
  }
#endif
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : BMC KCS base address decoding is done by IPMI module
  //
  // Disable the Watchdog timer expiration from causing a system reset
  //
  PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~0, B_PCH_PCR_ITSS_GIC_AME);

  //
  // Halt the TCO timer
  //
  Data16 = IoRead16 (PCH_TCO_BASE_ADDRESS + R_PCH_TCO1_CNT);
  Data16 |= B_PCH_TCO_CNT_TMR_HLT;
  IoWrite16 (PCH_TCO_BASE_ADDRESS + R_PCH_TCO1_CNT, Data16);

  //
  // Read the Second TO status bit
  //
  Data8 = IoRead8 (PCH_TCO_BASE_ADDRESS + R_PCH_TCO2_STS);
  if ((Data8 & B_PCH_TCO2_STS_SECOND_TO) == B_PCH_TCO2_STS_SECOND_TO) {
    TcoRebootHappened = 1;
    DEBUG ((EFI_D_INFO, "EarlyPlatformPchInit - TCO Second TO status bit is set. This might be a TCO reboot\n"));
  } else {
    TcoRebootHappened = 0;
  }

  //
  // Clear the Second TO status bit
  //
  Data8 |= (UINT8)  B_PCH_TCO2_STS_SECOND_TO;
  Data8 &= (UINT8) ~B_PCH_TCO2_STS_INTRD_DET;
  IoWrite8 (PCH_TCO_BASE_ADDRESS + R_PCH_TCO2_STS, Data8);

  //
  // Disable SERR NMI and IOCHK# NMI in port 61
  //
  Data8 = IoRead8 (R_PCH_NMI_SC);
  Data8 |= (B_PCH_NMI_SC_PCI_SERR_EN | B_PCH_NMI_SC_IOCHK_NMI_EN);
  IoWrite8 (R_PCH_NMI_SC, Data8);

  PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~B_PCH_PCR_ITSS_GIC_AME, 0);

  //
  // Clear EISS bit to allow for SPI use
  //
  MmioAnd8 (SpiBaseAddress + R_PCH_SPI_BC, (UINT8)~B_PCH_SPI_BC_EISS);

  FpkSelector(PeiServices, SystemBoard);
  Status = PchSoftStrapFixup(PeiServices, SystemBoard);
  if (Status == EFI_SUCCESS) {
    ResetRequired = TRUE;
  }


  DEBUG((EFI_D_ERROR, "EarlyPlatformPchInit - End\n"));

  if (ResetRequired == TRUE) {
    DEBUG((EFI_D_ERROR, "Performing global reset to block latch new PCH soft straps values...\n"));
    Status = (*PeiServices)->LocatePpi (
                              PeiServices,
                              &gPchResetPpiGuid,
                              0,
                              NULL,
                              (VOID **)&PchResetPpi
                              );
    ASSERT_EFI_ERROR (Status);
    PchResetPpi->Reset (PchResetPpi, GlobalReset);
  }
}

// APTIOV_SERVER_OVERRIDE_RC_START : Added PchResetPlatform() to call PchResetPpi->Reset
EFI_STATUS
EFIAPI
PchResetPlatform (
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:

  Provide hard reset PPI service.
  To generate full hard reset.

Arguments:

  PeiServices       General purpose services available to every PEIM.
  
Returns:
  
  Not return        System reset occurred.
  EFI_DEVICE_ERROR  Device error, could not reset the system.

--*/
{
  PCH_RESET_PPI   *PchResetPpi;
  EFI_STATUS      Status;


  Status = (*PeiServices)->LocatePpi (
                            PeiServices,
                            &gPchResetPpiGuid,
                            0,
                            NULL,
                            (VOID **)&PchResetPpi
                            );

  ASSERT_EFI_ERROR (Status);

  PchResetPpi->Reset (PchResetPpi, PowerCycleReset);

  return EFI_DEVICE_ERROR;
}
// APTIOV_SERVER_OVERRIDE_RC_END : Added PchResetPlatform() to call PchResetPpi->Reset
