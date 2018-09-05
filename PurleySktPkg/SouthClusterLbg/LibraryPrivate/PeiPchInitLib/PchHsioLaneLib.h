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
#ifndef _PCH_HSIO_LANE_LIB_H_
#define _PCH_HSIO_LANE_LIB_H_
#include "PchInitPei.h"

#define HSIO_JIM_STEP_MAX    32

/**
  Map PCIe lane index to HSIO lane number.

  @param[in] PcieLane   PCIe lane index (0-based)
  @return HSIO lane number
**/
UINT8
HsioPcieLane (
  UINT32 PcieLane
  );

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
  );

/**
  Write HSIO lane DWORD

  @param[in] HsioLane   HSIO Lane
  @param[in] Offset     PCR offset
  @param[in] Data32     DWORD Value
**/
VOID
HsioLaneWrite32 (
  CONST HSIO_LANE *HsioLane,
  UINT32          Offset,
  UINT32          Data32
  );

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
  UINT32          Offset,
  UINT32          AndMask,
  UINT32          OrMask
  );

/**
  Initialize HSIO lane for non-common clock PCIe port.

  @param[in] HsiLane   HSIO Lane
**/
VOID
HsioPcieNccLaneInit (
  CONST HSIO_LANE     *HsioLane
);

/**
  Toggle TAP_CFG_CTRL bit to apply configuration changes.

  @param[in] HsioLane   HSIO Lane
**/
VOID
HsioLaneToggleTapCfgCtrl (
  CONST HSIO_LANE     *HsioLane
  );

/**
  Freeze/unfreeze HSIO Lane calibration.

  @param[in] HsioLane   HSIO Lane
  @param[in] Freeze     TRUE to freeze, FALSE to unfreeze.
**/
VOID
HsioLaneFreeze (
  CONST HSIO_LANE   *HsioLane,
  BOOLEAN           Freeze,
  BOOLEAN           Ctoc
  );

/**
  Enable/disable HSIO lane Jitter Insertion Module

  @param[in] HsioLane   HSIO Lane
  @param[in] Enable     TRUE to enable, FALSE to disable.
**/
VOID
HsioLaneEnableJim (
  CONST HSIO_LANE   *HsioLane,
  BOOLEAN           Enable
  );

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
  );

/**
  Configure HSIO lane for software EQ.

  @param[in] HsioLane      HSIO Lane
**/
VOID
HsioLaneConfigSwEq (
  CONST HSIO_LANE *HsioLane
  );

#endif // _PCH_HSIO_LANE_LIB_H_
