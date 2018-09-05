/** @file
  Perform related functions for PCH Sata in DXE phase

@copyright
  Copyright (c) 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/
#include <PchInit.h>

/**
  Perform the remaining configuration on PCH SATA to perform device detection, 
  then set the SATA SPD and PxE corresponding, and set the Register Lock on PCH SATA 

  @retval None
**/
VOID
ConfigurePchSataOnEndOfDxe (
  IN UINT8 SataControllerNo
  )
{
  UINTN         PciSataRegBase;
  UINT16        SataPortsEnabled;
  PCH_SERIES    PchSeries;
  UINT16        WordReg;
  UINT32        DwordReg;
  UINTN         Index;

  ///
  /// SATA PCS: Enable the port in any of below condition:
  /// i.)   Hot plug is enabled
  /// ii.)  A device is attached
  /// iii.) Test mode is enabled
  /// iv.)  Configured as eSATA port
  ///
  UINT8                   MaxSataPortNum = 0;
  PCH_SATA_CONFIG         *SataConfig = NULL;
  PciSataRegBase = 0;


  if (SataControllerNo == PCH_SATA_FIRST_CONTROLLER) {
    PciSataRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA);
    MaxSataPortNum = GetPchMaxSataPortNum();
    SataConfig = &mPchPolicyHob->SataConfig;
  } else if (SataControllerNo == PCH_SATA_SECOND_CONTROLLER){
    PciSataRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SSATA, PCI_FUNCTION_NUMBER_PCH_SSATA);  
    MaxSataPortNum = GetPchMaxsSataPortNum();    
    SataConfig = &mPchPolicyHob->sSataConfig;    
  } else {
    DEBUG ((DEBUG_ERROR, "Error: Invalid SATA controller during SATA late DXE configuration!\n")); 
    ASSERT (FALSE);
  }

  PchSeries         = GetPchSeries ();
  SataPortsEnabled  = 0;
  if (PchSeries == PchLp) {
    WordReg           = MmioRead16 (PciSataRegBase + R_PCH_LP_SATA_PCS);
    for (Index = 0; Index < GetPchMaxSataPortNum (); Index++) {
      if ((mPchPolicyHob->SataConfig.PortSettings[Index].HotPlug == TRUE) ||
          (WordReg & (B_PCH_LP_SATA_PCS_P0P << Index)) ||
          (mPchPolicyHob->SataConfig.TestMode == TRUE) ||
          (mPchPolicyHob->SataConfig.PortSettings[Index].External == TRUE)) {
        SataPortsEnabled |= (mPchPolicyHob->SataConfig.PortSettings[Index].Enable << Index);
      }
    }

    ///
    /// Set MAP."Sata PortX Disable", SATA PCI offset 90h[10:8] to 1b if SATA Port 0/1/2 is disabled
    ///
    MmioOr16 (PciSataRegBase + R_PCH_LP_SATA_MAP, ((~SataPortsEnabled << N_PCH_LP_SATA_MAP_SPD) & (UINT16) B_PCH_LP_SATA_MAP_SPD));
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint16,
      (UINTN) (PciSataRegBase + R_PCH_LP_SATA_MAP),
      1,
      (VOID *) (UINTN) (PciSataRegBase + R_PCH_LP_SATA_MAP)
      );

    ///
    /// Program PCS "Port X Enabled", SATA PCI offset 92h[2:0] = Port 0~2 Enabled bit as per SataPortsEnabled value.
    ///
    MmioOr16 (PciSataRegBase + R_PCH_LP_SATA_PCS, SataPortsEnabled);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint16,
      (UINTN) (PciSataRegBase + R_PCH_LP_SATA_PCS),
      1,
      (VOID *) (UINTN) (PciSataRegBase + R_PCH_LP_SATA_PCS)
      );
  } else {
    DwordReg          = MmioRead32 (PciSataRegBase + R_PCH_H_SATA_PCS);
  for (Index = 0; Index < MaxSataPortNum; Index++) {
    if ((SataConfig->PortSettings[Index].HotPlug == TRUE) ||
        (DwordReg & (B_PCH_H_SATA_PCS_P0P << Index)) ||
        (SataConfig->TestMode == TRUE) ||
        (SataConfig->PortSettings[Index].External == TRUE)) {
      SataPortsEnabled |= (SataConfig->PortSettings[Index].Enable << Index);
    }
  }

    ///
    /// Set MAP."Sata PortX Disable", SATA PCI offset 90h[23:16] to 1b if SATA Port 0/1/2/3/4/5/6/7 is disabled
    ///
    MmioOr32 (PciSataRegBase + R_PCH_H_SATA_MAP, ((~SataPortsEnabled << N_PCH_H_SATA_MAP_SPD) & (UINT32) B_PCH_H_SATA_MAP_SPD));
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (PciSataRegBase + R_PCH_H_SATA_MAP),
      1,
      (VOID *) (UINTN) (PciSataRegBase + R_PCH_H_SATA_MAP)
      );

    ///
    /// Program PCS "Port X Enabled", SATA PCI offset 94h[7:0] = Port 0~7 Enabled bit as per SataPortsEnabled value.
    ///
    MmioOr16 (PciSataRegBase + R_PCH_H_SATA_PCS, SataPortsEnabled);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint16,
      (UINTN) (PciSataRegBase + R_PCH_H_SATA_PCS),
      1,
      (VOID *) (UINTN) (PciSataRegBase + R_PCH_H_SATA_PCS)
      );
  }

  ///
  /// Step 14
  /// Program SATA PCI offset 9Ch [31] to 1b
  ///
  MmioOr32 ((UINTN) (PciSataRegBase + R_PCH_SATA_SATAGC), BIT31);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (PciSataRegBase + R_PCH_SATA_SATAGC),
    1,
    (VOID *) (UINTN) (PciSataRegBase + R_PCH_SATA_SATAGC)
    );
}
