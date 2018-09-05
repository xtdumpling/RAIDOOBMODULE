/** @file
  HSIO lane routines

@copyright
  Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
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

**/
#include "PchHsioLaneLib.h"

/**
  Map PCIe lane index to HSIO lane number.

  @param[in] PcieLane   PCIe lane index (0-based)
  @return HSIO lane number
**/
UINT8
HsioPcieLane (
  UINT32 PcieLane
  )
{
  static UINT8 FirstLaneServer[]  = { 6, 10, 14, 18, 22 };

  UINT32       ControllerIndex;
  UINT32       ControllerLane;

  ControllerIndex = PcieLane / 4;
  ControllerLane  = PcieLane % 4;
  
  return FirstLaneServer[ControllerIndex] + (UINT8) ControllerLane;
}

/**
  Read HSIO lane DWORD

  @param[in] HsioLane   HSIO Lane
  @param[in] Offset     PCR offset
  @return DWORD value
**/
UINT32
HsioLaneRead32 (
  CONST HSIO_LANE *HsioLane,
  UINT32 Offset
  )
{
  UINT32      Data32;
  EFI_STATUS  Status;

  ASSERT ((Offset & 0x3) == 0);
  ASSERT (Offset < 0x200);

  Status = PchPcrRead32 (HsioLane->Pid, (UINT16) (HsioLane->Base + Offset), &Data32);
  ASSERT_EFI_ERROR (Status);

  return Data32;
}

/**
  Write HSIO lane DWORD

  @param[in] HsioLane   HSIO Lane
  @param[in] Offset     PCR offset
  @param[in] Data32     DWORD Value
**/
VOID
HsioLaneWrite32 (
  CONST HSIO_LANE *HsioLane,
  UINT32    Offset,
  UINT32    Data32
  )
{
  EFI_STATUS  Status;

  ASSERT ((Offset & 0x3) == 0);
  ASSERT (Offset < 0x200);

  Status = PchPcrWrite32 (HsioLane->Pid, (UINT16) (HsioLane->Base + Offset), Data32);
  ASSERT_EFI_ERROR (Status);
}

/**
  Modify HSIO lane DWORD.

  @param[in] HsioLane   HSIO Lane
  @param[in] Offset     PCR offset
  @param[in] AndMask    Mask to be ANDed with original value.
  @param[in] OrMask     Mask to be ORed with original value.
**/
VOID
HsioLaneAndThenOr32 (
  CONST HSIO_LANE *HsioLane,
  UINT32    Offset,
  UINT32    AndMask,
  UINT32    OrMask
  )
{
  UINT32 Data32;
  Data32 = HsioLaneRead32 (HsioLane, Offset);
  HsioLaneWrite32 (HsioLane, Offset, ((Data32 & AndMask) | OrMask));
}

/**
  Initialize HSIO lane for non-common clock PCIe port.

  @param[in] HsiLane   HSIO Lane
**/
VOID
HsioPcieNccLaneInit (
  CONST HSIO_LANE     *HsioLane
)
{
  UINT32  Data32Or;
  UINT32  Mask32;

  DEBUG ((DEBUG_INFO, "HsioPcieNccLaneInit(%d)\n", HsioLane->Index));

  Mask32    = B_PCH_HSIO_PCS_DWORD8_CRI_RXEB_LOWATER_4_0 |
              B_PCH_HSIO_PCS_DWORD8_CRI_RXEB_HIWATER_4_0;
  Data32Or  = ( 9 << N_PCH_HSIO_PCS_DWORD8_CRI_RXEB_LOWATER_4_0) |
              (24 << N_PCH_HSIO_PCS_DWORD8_CRI_RXEB_HIWATER_4_0);

  HsioLaneAndThenOr32 (HsioLane, R_PCH_HSIO_PCS_DWORD8, ~Mask32, Data32Or);
}

/**
  Toggle TAP_CFG_CTRL bit to apply configuration changes.

  @param[in] HsioLane   HSIO Lane
**/
VOID
HsioLaneToggleTapCfgCtrl (
  CONST HSIO_LANE     *HsioLane
  )
{
  HsioLaneAndThenOr32 (
    HsioLane,
    R_PCH_HSIO_RX_DWORD25,
    (UINT32) ~(B_PCH_HSIO_RX_DWORD25_RX_TAP_CFG_CTRL),
    0
    );
  HsioLaneAndThenOr32 (
    HsioLane,
    R_PCH_HSIO_RX_DWORD25,
    ~0u,
    B_PCH_HSIO_RX_DWORD25_RX_TAP_CFG_CTRL
    );
}

/**
  Freeze/unfreeze HSIO Lane calibration.

  @param[in] HsioLane   HSIO Lane
  @param[in] Freeze     TRUE to freeze, FALSE to unfreeze.
**/
VOID
HsioLaneFreeze (
  CONST HSIO_LANE  *HsioLane,
  BOOLEAN          Freeze,
  BOOLEAN          Ctoc
  )
{
  UINT32 Data32;

  Data32 = B_PCH_HSIO_RX_DWORD23_CFGVGATAP_ADAPT_OVERRIDE_EN;
  if (Ctoc) {
    Data32 |= B_PCH_HSIO_RX_DWORD23_ICFGVGABLWTAP_OVERRIDE_EN;
  }

  HsioLaneAndThenOr32 (
    HsioLane,
    R_PCH_HSIO_RX_DWORD23,
    ~Data32,
    Freeze ? Data32 : 0u
    );

  Data32 = B_PCH_HSIO_RX_DWORD9_CFGDFETAP1_OVERRIDE_EN |
           B_PCH_HSIO_RX_DWORD9_CFGDFETAP2_OVERRIDE_EN |
           B_PCH_HSIO_RX_DWORD9_CFGDFETAP3_OVERRIDE_EN |
           B_PCH_HSIO_RX_DWORD9_CFGDFETAP4_OVERRIDE_EN;
  HsioLaneAndThenOr32 (
    HsioLane,
    R_PCH_HSIO_RX_DWORD9,
    ~Data32,
    Freeze ? Data32 : 0u
    );

  HsioLaneToggleTapCfgCtrl (HsioLane);
}

/**
  Enable/disable HSIO lane Jitter Insertion Module

  @param[in] HsioLane   HSIO Lane
  @param[in] Enable     TRUE to enable, FALSE to disable.
**/
VOID
HsioLaneEnableJim (
  CONST HSIO_LANE  *HsioLane,
  BOOLEAN          Enable
  )
{
  HsioLaneAndThenOr32 (
    HsioLane,
    R_PCH_HSIO_RX_DWORD57,
    (UINT32) ~B_PCH_HSIO_RX_DWORD57_JIM_ENABLE,
    (Enable) ? B_PCH_HSIO_RX_DWORD57_JIM_ENABLE : 0
    );

  HsioLaneAndThenOr32 (
    HsioLane,
    R_PCH_HSIO_RX_DWORD56,
    (UINT32) ~B_PCH_HSIO_RX_DWORD56_ICFGPIDACCFGVALID,
    0
    );
  HsioLaneAndThenOr32 (
    HsioLane,
    R_PCH_HSIO_RX_DWORD56,
    ~0u,
    B_PCH_HSIO_RX_DWORD56_ICFGPIDACCFGVALID
    );
}

/**
  Configure JIM timers

  @param[in] HsioLane   HSIO Lane
  @param[in] Timer0     Timer 0 value.
  @param[in] Timer1     Timer 1 value.
  @param[in] Timer2     Timer 2 value.
  @param[in] Timer3     Timer 3 value.
**/
STATIC
VOID
HsioLaneSetJimTimers (
  CONST HSIO_LANE  *HsioLane,
  UINT16           Timer0,
  UINT16           Timer1,
  UINT16           Timer2,
  UINT16           Timer3
  )
{
  UINT32 Data32;

  ASSERT ((Timer0 & ~0xFF) == 0);
  ASSERT ((Timer1 & ~0xFF) == 0);
  ASSERT ((Timer2 & ~0xFF) == 0);
  ASSERT ((Timer3 & ~0x3FF) == 0);

  Data32 = ((Timer3 & 0xFF) << 24) |
           ((Timer2 & 0xFF) << 16) |
           ((Timer1 & 0xFF) <<  8) |
            (Timer0 & 0xFF);

  HsioLaneAndThenOr32 (
    HsioLane,
    R_PCH_HSIO_RX_DWORD57,
    (UINT32) ~(BIT21 | BIT20),
    (((Timer3 & 0x3FF) >> 8) << 20)
    );

  HsioLaneWrite32 (HsioLane, R_PCH_HSIO_RX_DWORD60, Data32);
}

/**
  Configure JIM module.

  @param[in] HsioLane      HSIO Lane
  @param[in] Step          JIM step
  @param[in] RecoveryWait  Recovery time
**/
VOID
HsioLaneSetJimClp (
  CONST HSIO_LANE  *HsioLane,
  UINT16           Step,
  UINT16           RecoveryWait
  )
{
  UINT16       Timer0, Timer1, Timer2, Timer3;
  UINT32       GainData32;

  Timer0 = Step;
  Timer1 = 1;
  Timer2 = 1;
  Timer3 = RecoveryWait - 5;

  HsioLaneSetJimTimers (HsioLane, Timer0, Timer1, Timer2, Timer3);

  if (Step == 0) {
    GainData32 = 0;
  } else {
    GainData32 = 4;
  }
  HsioLaneWrite32 (HsioLane, R_PCH_HSIO_RX_DWORD59, GainData32);

  HsioLaneAndThenOr32 (
    HsioLane,
    R_PCH_HSIO_RX_DWORD57,
    (UINT32) ~(B_PCH_HSIO_RX_DWORD57_JIM_COURSE | B_PCH_HSIO_RX_DWORD57_ICFGMARGINEN),
    B_PCH_HSIO_RX_DWORD57_JIMMODE
    );
}

/**
  Configure HSIO lane for software EQ.

  @param[in] HsioLane      HSIO Lane
**/
VOID
HsioLaneConfigSwEq (
  CONST HSIO_LANE *HsioLane
  )
{
  HsioLaneAndThenOr32 (
    HsioLane,
    R_PCH_HSIO_RX_DWORD8,
    (UINT32) ~(B_PCH_HSIO_RX_DWORD8_ICFGDFETAP3_EN),
    0
    );  
}

