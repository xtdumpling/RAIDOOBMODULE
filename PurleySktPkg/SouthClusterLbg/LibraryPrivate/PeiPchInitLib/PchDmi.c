/** @file
  This file contains functions for PCH DMI and OP-DMI configuration

@copyright
  Copyright (c) 2009 - 2015 Intel Corporation. All rights reserved
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

#include "PchInitPei.h"

#include <HeciRegs.h>
#include <MeChipset.h>

/**
  This function programs DMI miscellaneous registers.

  @param  None

  @retval None
**/
VOID
PchDmiAndOpDmiMiscProg (
  IN  PCH_POLICY_PPI       *PchPolicyPpi
  )
{
  UINT32                   Data32And;
  UINT32                   Data32Or;
  PCH_SERIES               PchSeries;

  PchSeries               = GetPchSeries ();

  ///
  /// PCH BIOS Spec, Section 7.2.4
  /// Step 2.1
  /// PCR[DMI] + 2088h = 00109000h
  ///
  PchPcrWrite32 (
    PID_DMI, R_PCH_PCR_DMI_UEM,
    0x00109000
    );

  ///
  /// Step 2.2
  /// PCR[DMI] + 20ACh[30] = 1b
  ///
  Data32Or = BIT30; // Unsupported Transaction Policy Bit (UTPB):
  Data32And = (UINT32)~0;
  //Stop and Scream Enable on DMI (HSD 4925618)
  if (PchPolicyPpi->DmiConfig.DmiStopAndScreamEnable) {
    Data32Or |= BIT31; // Drop Poisoned Downstream Packets (DPDP)
  }

  PchPcrAndThenOr32 (PID_DMI, R_PCH_PCR_DMI_REC, Data32And, Data32Or);


  // DMI transaction layer will not force a VC0 hang if is sees the physical layer report a hang condition
  if (PchPolicyPpi->DmiConfig.DmiLinkDownHangBypass == 1) {
        PchPcrAndThenOr32 (
            PID_DMI, R_PCH_PCR_DMI_2300,
            (UINT32)~0,
            BIT0
            );
      }
  if (PchSeries == PchLp) {
    ///
    /// Step 2.3
    /// Set PCR[DMI] + 2300h[1] = 1b
    ///
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_2300,
      (UINT32)~0,
      BIT1
      );

  } else {
    ///
    /// Step 2.4 and 2.5
    /// Set PCR[DMI] + 2348[3:0] = 0h
    ///
    Data32Or  = 0;
    Data32And = (UINT32) ~(BIT0 | BIT1 | BIT2 | BIT3);
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_2348,
      Data32And,
      Data32Or
      );
  }
}


/**
  This function configures DMI PM.

  @param[in] PchPolicyPpi The PCH Policy PPI instance

  @retval None
**/
VOID
ConfigureDmiAndOpDmiPm (
  IN  PCH_POLICY_PPI       *PchPolicyPpi
  )
{
  UINT32                   Data32And;
  UINT32                   Data32Or;
  PCH_SERIES               PchSeries;
  PCH_STEPPING             PchStep;

  PchSeries               = GetPchSeries ();
  PchStep                 = PchStepping ();

  ///
  /// Step 8.1
  /// Set PCR[DMI] + 2320h[6:4] = 001b
  ///
  Data32And = (UINT32)~(BIT6 | BIT5);
  Data32Or  = BIT4;
  PchPcrAndThenOr32 (
    PID_DMI, R_PCH_PCR_DMI_2320,
    Data32And,
    Data32Or
    );

  if (PchSeries == PchLp) {
    ///
    /// Step 8.2
    /// For SKL PCH-LP set PCR[DMI] + Offset 232Ch[0] = 1b (for LP B0 onwards)
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or  = BIT0;
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_232C,
      Data32And,
      Data32Or
      );
  } else if (PchSeries == PchH) {
    ///
    /// Step 8.2
    /// For SKL PCH-H set PCR[DMI] + Offset 232Ch[20] = 1b
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or = BIT20;
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_232C,
      Data32And,
      Data32Or
      );
  }

  if (PchSeries == PchH) {
    ///
    /// Step 8.3
    /// For SKL PCH-H set PCR[DMI] + 2320h [1] = 1b
    ///
    Data32And = 0xFFFFFFFF;;
    Data32Or  = BIT1;
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_2320,
      Data32And,
      Data32Or
      );
  }

  if (PchSeries == PchLp) {
    ///
    /// Step 8.4
    /// Set PCR[DMI] + 260Ch[15:0] = 0014h
    /// Set PCR[DMI] + 2618h[27] = 1b
    ///
    PchPcrWrite16 (
      PID_DMI, R_PCH_PCR_OPDMI_STC,
      (UINT16) 0x0014
      );
    }
    Data32And = 0xFFFFFFFF;
    Data32Or  = BIT27;
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_OPDMI_LCFG,
      Data32And,
      Data32Or
      );
    ///
    /// Step 8.5
    /// Set PCR[DMI] + 2614h[10] = 1b
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or  = (UINT32) BIT10;
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_OPDMI_LPMC,
      Data32And,
      Data32Or
      );
    ///
    /// Step 8.6-8.13
    /// Set PCR[DMI] + 2614h[30:25], [23:16], [8] = 111111b, 0x20, 1b respectively
    ///
    Data32And = (UINT32) ~(BIT23 | BIT22 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16);
    Data32Or  = (UINT32) (BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT21 | BIT10 | BIT8);
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_OPDMI_LPMC,
      Data32And,
      Data32Or
      );


    ///
    /// Step 8.15
    /// Set PCR[DMI] + 2600h[3] = 1b
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or  = BIT3;
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_OPDMI_LCTL,
      Data32And,
      Data32Or
      );


  if (PchSeries == PchH) {
    ///
    /// Step 8.16 and 8.17
    /// Set PCR[DMI] + 2304h[31:30,18:16] = 11b, 011b
    ///
    Data32And = (UINT32)~(BIT18);
    Data32Or  = (BIT31 | BIT30 | BIT17 | BIT16);
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_2304,
      Data32And,
      Data32Or
      );
    ///
    /// Step 8.18
    /// Set PCR[DMI] + 2478h[31:24] = 0x28
    ///
    Data32And = (UINT32)~(0xFF000000);
    Data32Or  = (UINT32) (0x28000000);
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_G3L0SCTL,
      Data32And,
      Data32Or
      );

    ///
    /// Step 8.19 and 8.20
    /// Set PCR[DMI] + 2344h[31:24] = 0x14
    /// Set PCR[DMI] + 2344h[23:16] = 0x14
    ///
    Data32And = (UINT32)~(0xFFFF0000);
    Data32Or  = (UINT32) (0x14140000);
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_2344,
      Data32And,
      Data32Or
      );

    ///
    /// Step 8.21
    /// Set PCR[DMI] + 21A4h[17:15] = 100b
    /// Step 8.22
    /// Set PCR[DMI] + 21A4h[14:12] = 100b
    ///
    Data32Or = BIT17 | BIT14;
    Data32And = (UINT32)~(B_PCH_PCR_DMI_LCAP_EL1 | B_PCH_PCR_DMI_LCAP_EL0);
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_LCAP,
      Data32And,
      Data32Or
      );

    ///
    /// Step 8.23.1
    /// Set PCR[DMI] + 2340h[7:0] = 2Dh
    /// Step 8.23.2
    /// Set PCR[DMI] + 2340h[23:16] = 5Bh
    ///
    Data32And = (UINT32) 0xFF00FF00;
    Data32Or  = (UINT32) ((0x5B << 16) | 0x2D);
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_2340,
      Data32And,
      Data32Or
      );

    ///
    /// Step 8.23.3
    /// Set PCR[DMI] + Offset 2478h[7:0] = 2Ch
    ///
    Data32And = (UINT32) 0xFFFFFF00;
    Data32Or  = (UINT32) (0x2C);
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_G3L0SCTL,
      Data32And,
      Data32Or
      );

    ///
    /// Retrain DMI Link
    ///
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_LCTL,
      0xFFFFFFFF,
      (UINT32) BIT5
      );

    ///
    /// Step 8.24
    /// Set PCR[DMI] + 2310h[30,27] = 1b, 1b
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or  = (BIT30 | BIT27);
    PchPcrAndThenOr32 (
      PID_DMI,R_PCH_PCR_DMI_2310,
      Data32And,
      Data32Or
      );

    ///
    /// Step 8.25 - 8.27
    /// Set PCR[DMI] + 2314h[23,22:21,5] = 0b, 01b, 1b
    ///
    Data32And = (UINT32)~(BIT23 | BIT22);
    Data32Or  = (BIT21 | BIT5);
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_2314,
      Data32And,
      Data32Or
      );

    ///
    /// Step 8.28
    /// Set PCR[DMI] + 2310h[21] = 0b
    ///
    Data32And = (UINT32)~(BIT21);
    Data32Or  = 0;
    PchPcrAndThenOr32 (
      PID_DMI,R_PCH_PCR_DMI_2310,
      Data32And,
      Data32Or
      );

    ///
    /// Step 8.29
    /// Set PCR[DMI] + 2238h[0] = 1b
    /// Set PCR[DMI] + 2238h[11:10] = 10b
    /// Set PCR[DMI] + 2238h[9:8] = 01b
    ///
    Data32And = (UINT32) ~(BIT9 | BIT10);
    Data32Or  = (UINT32)(BIT11 | BIT8 | BIT0);
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_DMIHWAWC,
      Data32And,
      Data32Or
      );

    ///
    /// Step 8.30 - 8.31
    /// Set PCR[DMI] + 2334[8,0] = 1h,1h
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or  = BIT8 | BIT0;
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_2334,
      Data32And,
      Data32Or
      );
  }

  ///
  /// Step 8.32
  /// Set PCR[DMI] + 2334[1] = 1h
  ///
  Data32And = 0xFFFFFFFF;
  Data32Or  = BIT1;
  PchPcrAndThenOr32 (
    PID_DMI, R_PCH_PCR_DMI_2334,
    Data32And,
    Data32Or
    );
  ///
  /// Step 8.33 - 8.34
  /// Set PCR[DMI] + 234C[5] = 1b and clear [2:0]=000b
  ///
  Data32And = (UINT32)~(BIT2 | BIT1 | BIT0);
  Data32Or  = (BIT5);
  PchPcrAndThenOr32 (
    PID_DMI, R_PCH_PCR_DMI_234C,
    Data32And,
    Data32Or
    );

}

/**
  This function configures DMI and OP-DMI clock gating

  @param[in] PchPolicyPpi The PCH Policy PPI instance

  @retval None
**/
VOID
ConfigureDmiAndOpDmiClockGating (
  IN  PCH_POLICY_PPI  *PchPolicyPpi
  )
{

  ///
  /// Step 8.35 - 8.42
  /// Enable Dynamic Clock Gating by programming below registers
  /// before enabling ASPM.
  /// PCR[DMI] + 2234h [4:0] to 11111b (using byte access)
  /// PCR[DMI] + 223Eh [6,5,3:2] to 1b,1b,00b
  ///
  PchPcrAndThenOr8 (
    PID_DMI, R_PCH_PCR_DMI_DMIC,
    (UINT8)0xFF,
    (UINT8)(BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
    );

  PchPcrAndThenOr8 (
    PID_DMI, R_PCH_PCR_DMI_IOSFSBCS,
    (UINT8)~(BIT3 | BIT2),
    (UINT8)(BIT6 | BIT5)
    );

}


/**
  This function configures ASPM on DMI

  @param[in] PchPolicyPpi The PCH Policy PPI instance

  @retval None
**/
VOID
ConfigureDmiAspm (
  IN  PCH_POLICY_PPI  *PchPolicyPpi
  )
{
  UINT32                        Data32And;
  UINT32                        Data32Or;
  UINT32                        Data32;
  UINT16                        Data16And;
  UINT16                        Data16Or;
  PCH_PCIE_ASPM_CONTROL         DmiAspmCtrl;

  ///
  /// Enable DMI ASPM
  ///
  if (PchPolicyPpi->DmiConfig.DmiAspm == TRUE) {
    ///
    /// While DmiAspm is enabled, DMI ASPM will be set to Intel recommended value.
    /// PCH BIOS Spec, Section 8.3.1 ASPM on DMI and the PCI Express* Root Ports
    /// Note: We recommend PCH platforms to enable L0s and L1, but unless both sides of the link have L0s and/or
    /// L1 enabled they will be disabled by the link.
    ///
    DmiAspmCtrl = PchPcieAspmL1;
  } else {
    DmiAspmCtrl = PchPcieAspmDisabled;
  }
  
  PchPcrRead32 (PID_DMI, R_PCH_PCR_DMI_DMIC, &Data32);
  
  if((Data32 & B_PCH_PCR_DMI_DMIC_DNPRL) == B_PCH_PCR_DMI_DMIC_DNPRL) {
    ///
    /// If DNPRL bit is set HALEP should be cleared
    ///
    Data32And = (UINT32)~(B_PCH_PCR_DMI_2310_HALEP);
    Data32Or = 0;
  } else {
    ///
    /// If DNPRL bit is not set HALEP should be set
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or = (UINT32)(B_PCH_PCR_DMI_2310_HALEP);
  }

    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_2310,
      Data32And,
      Data32Or
      );


  if (DmiAspmCtrl != PchPcieAspmDisabled) {
    ///
    /// PCH BIOS Spec Section 8.3
    /// BIOS should set PCR[DMI] + 2304h[10] to 0b prior to enabling DMI ASPM.
    ///
    Data32And = (UINT32)~(BIT10);
    Data32Or  = 0;
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_2304,
      Data32And,
      Data32Or
      );

    ///
    /// PCH BIOS Spec, Section 8.3
    ///
    /// Step 1
    /// PCR[DMI] + 21A4h[11:10] = 11b
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or  = B_PCH_PCR_DMI_LCAP_APMS;
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_LCAP,
      Data32And,
      Data32Or
      );

    if (DmiAspmCtrl == PchPcieAspmL1) {
      ///
      /// Step 2
      /// Enable L1 Entry only by setting PCR[DMI]+ offset 21A8h[1:0] to 10b
      ///
      Data16And = (UINT16) (~(BIT0));
      Data16Or  = (UINT16) BIT1;

    } else {
      //
      // Do nothing
      //
      Data16And = 0xFFFF;
      Data16Or  = 0;
    }

    ///
    /// Program PCR[DMI] + 21A8h[1:0]
    ///
    PchPcrAndThenOr16 (
      PID_DMI, R_PCH_PCR_DMI_LCTL,
      Data16And,
      Data16Or
      );
    ///
    /// BIOS should set PCR[DMI] + 2304h[10] back to 1b after enabling DMI ASPM.
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or  = (UINT32) (BIT10);
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_DMI_2304,
      Data32And,
      Data32Or
      );

  }
}

/**
  This function configures DMI and OP-DMI.

  @param[in] PchPolicyPpi The PCH Policy PPI instance

  @retval None
**/
VOID
PchDmiAndOpDmiInit (
  IN  PCH_POLICY_PPI  *PchPolicyPpi
  )
{
  PCH_SERIES               PchSeries;

  PchSeries               = GetPchSeries ();

    PchDmiAndOpDmiMiscProg (PchPolicyPpi);


  ConfigureDmiAndOpDmiPm (PchPolicyPpi);

  ConfigureDmiAndOpDmiClockGating (PchPolicyPpi);

  if (PchSeries == PchH) {
    ConfigureDmiAspm (PchPolicyPpi);
  } else {

    ///
    /// Step 10.1
    /// Set PCR[DMI] + 2618h [25] = 1b.
    /// Setting this lock bit can be done in PEI as registers which it locks
    /// do not get programmed later in BIOS. Another lock bit DMIC.SRL (PCR[DMI] + 2234h[31])
    /// can't be set in PEI as this would break ActiveBIOS module.
    ///
    PchPcrAndThenOr32 (
      PID_DMI, R_PCH_PCR_OPDMI_LCFG,
      (UINT32)0xFFFFFFFF,
      (UINT32)BIT25
      );
  }
}
