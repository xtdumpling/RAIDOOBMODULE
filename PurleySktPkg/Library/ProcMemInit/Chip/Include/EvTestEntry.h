/** @file
  This file contains the definition of the BIOS implementation of the BIOS-SSA Common Configuration API.

@copyright
  Copyright (c) 2015 Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#ifndef __EV_TEST_ENTRY_H__
#define __EV_TEST_ENTRY_H__

#ifdef SSA_FLAG

#include "SysFunc.h"
#include "EvTypes.h"

#ifndef RMT_CONFIG

#define RMT_CONFIG_GUID \
{0x17C12D32,0xB35A,0x4E1A,{0xBE,0x5D,0xC4,0xAB,0x29,0x29,0xF5,0xA6} }

#pragma pack (push, 1)

typedef struct RMT_CONFIG{
  UINT32                  SocketBitmask;
  UINT8                   ControllerBitmask;
  UINT8                   IoLevelBitmask;
  BOOLEAN                 EnableRankMargin;
  BOOLEAN                 EnableCtlAllMargin;
  BOOLEAN                 EnableEridMargin;
  BOOLEAN                 EnableLaneMargin;
  BOOLEAN                 EnableTurnaroundMargin;
  UINT8                   RxDqsStepSize;
  UINT8                   TxDqStepSize;
  UINT8                   RxVrefStepSize;
  UINT8                   TxVrefStepSize;
  UINT8                   CmdAllStepSize;
  UINT8                   CmdVrefStepSize;
  UINT8                   CtlAllStepSize;
  UINT8                   EridDelayStepSize;
  UINT8                   EridVrefStepSize;
  UINT16                  SinglesBurstLength;
  UINT32                  SinglesLoopCount;
  UINT32                  SinglesBinarySearchLoopCount;
  UINT16                  TurnaroundsBurstLength;
  UINT32                  TurnaroundsLoopCount;
  UINT32                  TurnaroundsBinarySearchLoopCount;
  UINT16                  OptimizedShmooBitmask;
  SCRAMBLER_OVERRIDE_MODE ScramblerOverrideMode;
  BOOLEAN                 EnableExtendedRange;
  BOOLEAN                 EnableTestResultLogging;
  BOOLEAN                 EnableLaneMarginPlot;
  UINT8                   TestStatusLogLevel;
}RMT_CONFIG;

#pragma pack (pop)

#endif // RMT_CONFIG
#endif //SSA_FLAG
#endif //__EV_TEST_ENTRY_H__
