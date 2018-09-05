//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  AdddcSparing.h

Abstract:

  This file defines the macros and function prototypes used for ADDDC sparing

**/

#ifndef _DOUBLE_BANK_DDDC_H_
#define _DOUBLE_BANK_DDDC_H_

#include <MCDDC_DP.h>
#include <Protocol/MemRasProtocol.h>
#include "MemRas.h"
#include "DramDeviceCorrection.h"
#include "RankSparing.h"
#include "SparingEngine.h"


EFI_STATUS
EFIAPI
InitializeAdddcMR(
    );

BOOLEAN
AnyPreviousBuddy(
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    IN UINT8 BankValid,
    OUT UINT8 *PrevRegion,
    OUT UINT8 *PrevStrike
    );


BOOLEAN
AnyPreviousStrike (
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    IN UINT8 Gran,
    OUT UINT8 *PrevStrike
    );

BOOLEAN
AnyPreviousStrikeMatch (
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    IN UINT8 Gran,
    OUT UINT8 *PrevStrike
    );

EFI_STATUS
EFIAPI
ClearAdddcStatusStruct(IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    OUT BOOLEAN *IsEventHandled
    );

EFI_STATUS
EFIAPI
UpgradeBank2Rank (IN UINT8 NodeId,
                  IN UINT8 ddrCh,
                  IN UINT8 StrikeNum,
                  IN UINT8 RegionNum,
                  OUT BOOLEAN *IsEventHandled
                  );

BOOLEAN
EFIAPI
IsRankInPlusOne(
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 rank
    );

EFI_STATUS
EFIAPI
ReverseRemapBank (IN UINT8 NodeId,
                  IN UINT8 ddrCh,
                  IN UINT8 CurrentStrikeNum,
                  IN UINT8 PrevStrikeNum,
                  IN UINT8 PrevRegionNum,
                  OUT BOOLEAN *IsEventHandled
                  );

EFI_STATUS
EFIAPI
ReverseRemapRank (IN UINT8 NodeId,
                  IN UINT8 ddrCh,
                  IN UINT8 CurrentStrikeNum,
                  IN UINT8 PrevStrikeNum,
                  IN UINT8 PrevRegionNum,
                  OUT BOOLEAN *IsEventHandled
                  );


EFI_STATUS
EFIAPI
BankADDDCMR (
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    IN UINT8 RegionNumInput,
    OUT BOOLEAN *IsEventHandled
    );

EFI_STATUS
EFIAPI
RankADDDCMR (
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    IN UINT8 RegionNumInput,
    OUT BOOLEAN *IsEventHandled
    );

VOID
PopulateFailedDevInfo(
    IN UINT8 NodeId,
    IN UINT8 ddrCh,
    IN UINT8 StrikeNum,
    IN PFAILED_DIMM_INFO pFailedDimmInfo
    );

EFI_STATUS
EFIAPI
CheckandHandleNewAdddcMREvent(
    IN  UINT8 NodeId,
    OUT BOOLEAN *IsEventHandled
 );

EFI_STATUS
EFIAPI
HandleInProgressAdddcMREvent(
    IN  UINT8   NodeId,
    OUT BOOLEAN *IsEventHandled
 );

BOOLEAN
IsAdddcAvailable(
    IN UINT8  NodeId,
    IN UINT8  ddrCh
 );

#endif //_DOUBLE_BANK_DDDC_H_



