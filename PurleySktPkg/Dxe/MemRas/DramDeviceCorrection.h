//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file DramDeviceCorrection.h

  This file defines the macros and function prototypes used for DRAM device correction

**/

#ifndef _DRAM_DEVICE_CORRECTION_H_
#define _DRAM_DEVICE_CORRECTION_H_

#include <Protocol/MemRasProtocol.h>
#include "MemRas.h"
#include "RankSparing.h"
#include "AdddcSparing.h"

#define   X8DIMM_VLS      0
#define   X8DIMM_NoVLS    1
#define   X4DIMM_ADDDC    2
#define   X4DIMM_NoADDDC    3

EFI_STATUS
EFIAPI
InitializeSddcPlusOne(
 );

EFI_STATUS
EFIAPI
HandleNewSddcPlusOneEventsIfAny(
    IN UINT8 NodeId,
    OUT BOOLEAN *IsEventHandled
 );

EFI_STATUS
EFIAPI
HandleSddcPlusOneEventDone(
    IN  UINT8    NodeId,
    OUT BOOLEAN *IsEventHandled
);

BOOLEAN
IsSddcPlusOneAvailable(
    IN UINT8  NodeId,
    IN UINT8  DdrCh,
    IN UINT8  Rank
 );

BOOLEAN
IsThisRankInADDDCRegion(
    IN UINT8 NodeId,
    IN UINT8 DdrCh,
    IN UINT8 Rank, 
    OUT UINT8 *RegionNum,
    OUT UINT8 *Plus1BuddyRank
    );
EFI_STATUS
EFIAPI
StartSddcPlusOne (
    IN  UINT8 NodeId,
    IN  UINT8 ddrCh,
    IN  UINT8 Rank,
    OUT BOOLEAN *IsEventHandled
  );
#endif //_DRAM_DEVICE_CORRECTION_H_
