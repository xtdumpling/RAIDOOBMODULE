/** @file
  This files contains Pch DMI services for RCs usage

@copyright
  Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
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

#include "PchRcLibrary.h"

/**
  Get PCH default TC VC Mapping settings. This funciton returns the default PCH setting
  System Agent can update the settings according to polices.

  @param[in, out] PchDmiTcVcMap         Buffer for PCH_DMI_TC_VC_MAP instance.

**/
VOID
PchDmiTcVcMapInit (
  IN OUT  PCH_DMI_TC_VC_MAP             *PchDmiTcVcMap
  )
{
  if (PchDmiTcVcMap == NULL) {
    return;
  }

  PchDmiTcVcMap->DmiTc[DmiVcTypeVc0].Vc   = DmiVcTypeVc0;
  PchDmiTcVcMap->DmiTc[DmiVcTypeVc0].TcId = 0;
  PchDmiTcVcMap->DmiTc[DmiVcTypeVc1].Vc   = DmiVcTypeVc1;
  PchDmiTcVcMap->DmiTc[DmiVcTypeVc1].TcId = 1;
  PchDmiTcVcMap->DmiTc[DmiVcTypeVcm].Vc   = DmiVcTypeVcm;
  PchDmiTcVcMap->DmiTc[DmiVcTypeVcm].TcId = 7;

  PchDmiTcVcMap->DmiVc[0].Enable          = TRUE;
  PchDmiTcVcMap->DmiVc[0].VcId            = 0;
  PchDmiTcVcMap->DmiVc[1].Enable          = TRUE;
  PchDmiTcVcMap->DmiVc[1].VcId            = 1;
  PchDmiTcVcMap->DmiVc[2].Enable          = TRUE;
  PchDmiTcVcMap->DmiVc[2].VcId            = 7;
}

/**
  Programing transaction classes of the corresponding virtual channel and Enable it

  @param[in] Vc                   The virtual channel number for programing
  @param[in] VcId                 The Identifier to be used for this virtual channel
  @param[in] VcMap                The transaction classes are mapped to this virtual channel.
                                  When a bit is set, this transaction class is mapped to the virtual channel

  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
PchSetDmiTcVcMapping (
  IN   UINT8            Vc,
  IN   UINT8            VcId,
  IN   UINT8            VcMap
  )
{
  UINT16  Address;
  UINT32  VxCtlAnd;
  UINT32  VxCtlOr;


  VxCtlAnd  = (UINT32) (~(B_PCH_PCR_DMI_V1CTL_ID | V_PCH_PCR_DMI_V1CTL_TVM_MASK));
  VxCtlOr   = ((UINT32)VcId << N_PCH_PCR_DMI_V1CTL_ID) & B_PCH_PCR_DMI_V1CTL_ID;
  VxCtlOr |= (UINT32)VcMap;
  VxCtlOr |= B_PCH_PCR_DMI_V1CTL_EN;

  switch (Vc) {
  case DmiVcTypeVc0:
    Address = R_PCH_PCR_DMI_V0CTL;
    break;

  case DmiVcTypeVc1:
    Address = R_PCH_PCR_DMI_V1CTL;
    break;

  case DmiVcTypeVcm:
    Address = R_PCH_PCR_DMI_VMCTL;
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }

  PchPcrAndThenOr32 (PID_DMI, Address, VxCtlAnd, VxCtlOr);
  //
  // Reads back for posted write to take effect
  // Read back is done in PchPcr lib
  //

  return EFI_SUCCESS;
}

/**
  Polling negotiation status of the corresponding virtual channel

  @param[in] Vc                   The virtual channel number for programing

  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
PchPollDmiVcStatus (
  IN   UINT8            Vc
  )
{
  UINT16  Address;
  UINT16  Data16;

  switch (Vc) {
  case DmiVcTypeVc0:
    Address = R_PCH_PCR_DMI_V0STS;
    break;

  case DmiVcTypeVc1:
    Address = R_PCH_PCR_DMI_V1STS;
    break;

  case DmiVcTypeVcm:
    Address = R_PCH_PCR_DMI_VMSTS;
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }
  //
  // Wait for negotiation to complete
  //
  PchPcrRead16 (PID_DMI, Address, &Data16);
  while ((Data16 & B_PCH_PCR_DMI_V1STS_NP) != 0) {
    PchPcrRead16 (PID_DMI, Address, &Data16);
  }

  return EFI_SUCCESS;
}

/**
 The function performing TC/VC mapping program

 @param[in] PchDmiTcVcMap              Buffer for PCH_DMI_TC_VC_MAP instance.

 @retval EFI_SUCCESS                   The function completed successfully
 @retval Others                        All other error conditions encountered result in an ASSERT.
 **/
EFI_STATUS
EFIAPI
PchDmiTcVcProg (
   IN PCH_DMI_TC_VC_MAP *PchDmiTcVcMap
  ) 
{
  UINT8 Index;
  UINT8 VcMap[DmiVcTypeMax] = { 0 };


  ///
  /// Step 3.3, Step 3.4, Step 3.5, Step 3,6, Set the TC/VC mappings
  ///
  for (Index = 0; Index < DmiTcTypeMax; Index++) {
    DEBUG((DEBUG_INFO, "TC:%0x VC:%0x!\n", PchDmiTcVcMap->DmiTc[Index].TcId, PchDmiTcVcMap->DmiTc[Index].Vc));
     VcMap[PchDmiTcVcMap->DmiTc[Index].Vc] |= (BIT0 << PchDmiTcVcMap->DmiTc[Index].TcId);
  }

  for (Index = 0; Index < DmiVcTypeMax; Index++) {
    DEBUG((DEBUG_INFO, "VC:%0x VCID:%0x Enable:%0x!\n",Index, PchDmiTcVcMap->DmiVc[Index].VcId, PchDmiTcVcMap->DmiVc[Index].Enable));
    if (PchDmiTcVcMap->DmiVc[Index].Enable == TRUE ) {
        PchSetDmiTcVcMapping(
                             Index, 
                             PchDmiTcVcMap->DmiVc[Index].VcId,
                             VcMap[Index]
                             );
    }
  }

  return EFI_SUCCESS;
}

/**
 The function poll all PCH Virtual Channel until negotiation completion.

 @param[in] PchDmiTcVcMap              Buffer for PCH_DMI_TC_VC_MAP instance.

 @retval EFI_SUCCESS                   The function completed successfully
 @retval Others                        All other error conditions encountered result in an ASSERT.
 **/
EFI_STATUS
EFIAPI
PchDmiTcVcPoll (
  IN PCH_DMI_TC_VC_MAP *PchDmiTcVcMap
  ) 
{
  UINT8 Index;
  ///
  /// Step 3.8
  /// After both above and System Agent DMI TC/VC mapping are programmed,
  /// poll VC negotiation pending status until is zero:
  /// 3.8.1 PCR[DMI] + Offset 201Ah[1]
  /// 3.8.2 PCR[DMI] + Offset 2026h[1]
  /// 3.8.4 PCR[DMI] + Offset 2046h[1]
  ///
  for (Index = 0; Index < DmiVcTypeMax; Index++) {
     if (PchDmiTcVcMap->DmiVc[Index].Enable == TRUE ) {
          PchPollDmiVcStatus(Index);
     }
  }

  return EFI_SUCCESS;
}

/**
  This function will perform necessary programming before changing PCH DMI link speed to Gen2 or Gen3.

  @param[in] DmiSpeedGen              PCH DMI speed Gen (1-Gen1, 2-Gen2, 3-Gen3) to which link is going to be trained

  @retval none
**/
STATIC
VOID
PchDmiBeforeLinkSpeedChange (
  UINT8    DmiSpeedGen
  )
{
  UINT32            Data32Or;
  UINT32            Data32And;
  if (DmiSpeedGen > 2) {
    ///
    /// Program Upstream Port Lane 0-3 Transmitter Preset
    ///
    Data32And = ~(UINT32) (B_PCH_PCR_DMI_UPL02RPH | B_PCH_PCR_DMI_UPL13RPH);
    Data32Or = ((V_PCH_PCR_DMI_UPL0RPH << N_PCH_PCR_DMI_UPL02RPH) | (V_PCH_PCR_DMI_UPL1RPH << N_PCH_PCR_DMI_UPL13RPH));

    PchPcrAndThenOr32 (PID_DMI, R_PCH_PCR_DMI_L01EC, Data32And, Data32Or);

    Data32And = ~(UINT32) (B_PCH_PCR_DMI_UPL02RPH | B_PCH_PCR_DMI_UPL13RPH);
    Data32Or  = ((V_PCH_PCR_DMI_UPL2RPH << N_PCH_PCR_DMI_UPL02RPH) | (V_PCH_PCR_DMI_UPL3RPH << N_PCH_PCR_DMI_UPL13RPH));
    PchPcrAndThenOr32 (PID_DMI, R_PCH_PCR_DMI_L23EC, Data32And, Data32Or);

    ///
    /// Set PCR[DMI] + 2450h[16] = 1b
    ///
    PchPcrAndThenOr32 (PID_DMI, R_PCH_PCR_DMI_EQCFG1, (UINT32)~0, (UINT32)(BIT16));
  }

  ///
  /// Set  DMI Link Data Rate Sustain Policy (DLDRSP) i.e. PCR[DMI] + 2338[9] = 1h
  /// DLDRSP should be set to 1 when the Target Link Speed is still indicating GEN1
  ///
  PchPcrAndThenOr32 (
    PID_DMI, R_PCH_PCR_DMI_2338,
    ~(UINT32) 0,
    (UINT32) BIT9
    );
}

/**
  The function set the Target Link Speed in PCH to DMI GEN 2.

**/
VOID
EFIAPI
PchDmiGen2Prog (
  VOID
  )
{
  UINT32   Data32;

  if (GetPchSeries () == PchH) {
    DEBUG ((DEBUG_INFO, "PchDmiGen2Prog() Start\n"));

    PchDmiBeforeLinkSpeedChange (2);

    ///
    /// PCH BIOS Spec, Section 7.2.4
    /// Step 5
    /// Configure DMI Link Speed (for PCH With DMI ONLY)
    /// Step 5.1
    /// Please refer to the System Agent BIOS Writer's Guide on Supported Link Speed
    /// field in Link Capabilities register in CPU complex. (Done in SA code)
    /// Step 5.2
    /// If target link speed is GEN2, it can be configured as early as possible.
    /// Else if the target link speed is GEN3, refer to section 7.1 for further programming requirement
    /// Step 5.3
    /// Set PCR[DMI] + 21B0h[3:0] = AND(Supported Link Speed in CPU complex, PCR[DMI] + 21A4h[3:0])
    /// If the Supported Link Speed in CPU complex is 0010b or above(Done in SA code)
    /// and PCR[DMI] + Offset 21A4h[3:0] >= 0010b
    ///
    PchPcrRead32 (PID_DMI, R_PCH_PCR_DMI_LCAP, &Data32);
    if ((Data32 & B_PCH_PCR_DMI_LCAP_MLS) >= 0x02) {
      ///
      /// Step 2.2.1
      /// Set PCR[DMI] + Offset 21B0h[3:0] = 0010b
      ///
      PchPcrAndThenOr8 (PID_DMI, R_PCH_PCR_DMI_LCTL2, (UINT8)~(BIT3 | BIT2 | BIT1 | BIT0), (UINT8) BIT1);
      ///
      /// Step 2.2.2
      /// Please refer to the System Agent BIOS Writer's Guide to perform DMI Link Retrain after
      /// configures new DMI Link Speed. (Done in SA code)
      ///
    }
    DEBUG ((DEBUG_INFO, "PchDmiGen2Prog() End\n"));
  }

}

/**
  The function set the Target Link Speed in PCH to DMI GEN 3.

  @retval None
**/
VOID
EFIAPI
PchDmiGen3Prog (
  VOID
  )
{
  UINT32  Data32;

  if (GetPchSeries () == PchH) {
    DEBUG ((DEBUG_INFO, "PchDmiGen3Prog() Start\n"));

    PchDmiBeforeLinkSpeedChange (3);

    ///
    /// PCH BIOS Spec, Section 7.2.4
    /// Step 5
    /// Configure DMI Link Speed (for PCH With DMI ONLY)
    /// Step 5.1
    /// Please refer to the System Agent BIOS Writer's Guide on Supported Link Speed
    /// field in Link Capabilities register in CPU complex. (Done in SA code)
    /// Step 5.2
    /// If target link speed is GEN2, it can be configured as early as possible.
    /// Else if the target link speed is GEN3, refer to section 7.1 for further programming requirement
    /// Step 5.3
    /// Set PCR[DMI] + 21B0h[3:0] = AND(Supported Link Speed in CPU complex, PCR[DMI] + 21A4h[3:0])
    /// If the Supported Link Speed in CPU complex is 0011b (Done in SA code)
    /// and PCR[DMI] + Offset 21A4h[3:0] = 0011b
    ///
    PchPcrRead32 (PID_DMI, R_PCH_PCR_DMI_LCAP, &Data32);
    if ((Data32 & B_PCH_PCR_DMI_LCAP_MLS) == 0x03) {
       ///
       /// HSD 5332537: LBG DMI Tx co-efficient update
       ///
       PchPcrAndThenOr32 (PID_DMI, R_PCH_PCR_DMI_EQCFG1, (UINT32)~(0), (UINT32) (B_PCH_PCR_DMI_EQCFG1_RTLEPCEB | B_PCH_PCR_DMI_EQCFG1_RTPCOE));
       PchPcrAndThenOr32 (PID_DMI, R_PCH_PCR_DMI_RTPCL1,
                           (UINT32)~(B_PCH_PCR_DMI_RTPCL1_RTPRECL2PL4  |
                                     B_PCH_PCR_DMI_RTPCL1_RTPOSTCL1PL3 |
                                     B_PCH_PCR_DMI_RTPCL1_RTPRECL1PL2  |
                                     B_PCH_PCR_DMI_RTPCL1_RTPOSTCL0PL1 |
                                     B_PCH_PCR_DMI_RTPCL1_RTPRECL0PL0
                                    ),
                           (UINT32) (B_PCH_PCR_DMI_RTPCL1_PCM                  |
                                     (10 << N_PCH_PCR_DMI_RTPCL1_RTPRECL2PL4)  |
                                     ( 8 << N_PCH_PCR_DMI_RTPCL1_RTPOSTCL1PL3) |
                                     (10 << N_PCH_PCR_DMI_RTPCL1_RTPRECL1PL2)  |
                                     ( 8 << N_PCH_PCR_DMI_RTPCL1_RTPOSTCL0PL1) |
                                     (10 << N_PCH_PCR_DMI_RTPCL1_RTPRECL0PL0)
                                    )
                          );
       PchPcrAndThenOr32 (PID_DMI, R_PCH_PCR_DMI_RTPCL2,
                           (UINT32)~(B_PCH_PCR_DMI_RTPCL2_RTPOSTCL2PL5 |
                                     B_PCH_PCR_DMI_RTPCL2_RTPRECL3PL6  |
                                     B_PCH_PCR_DMI_RTPCL2_RTPOSTCL3PL7
                                    ),
                           (UINT32) (( 8 << N_PCH_PCR_DMI_RTPCL2_RTPOSTCL2PL5) |
                                     (10 << N_PCH_PCR_DMI_RTPCL2_RTPRECL3PL6 ) |
                                     ( 8 << N_PCH_PCR_DMI_RTPCL2_RTPOSTCL3PL7)
                                    )
                          );

      ///
      /// Set PCR[DMI] + Offset 21B0h[3:0] = 0011b
      ///
      PchPcrAndThenOr8 (PID_DMI, R_PCH_PCR_DMI_LCTL2, (UINT8)~(BIT3 | BIT2 | BIT1 | BIT0), (UINT8) (BIT1 | BIT0));
      ///
      /// Please refer to the System Agent BIOS Writer's Guide to perform DMI Link Retrain after
      /// configures new DMI Link Speed. (Done in SA code)
      ///
    }
    DEBUG ((DEBUG_INFO, "PchDmiGen3Prog() End\n"));
  }

}
