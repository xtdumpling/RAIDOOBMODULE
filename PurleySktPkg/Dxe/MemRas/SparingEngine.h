//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SparingEngine.h

  This file defines the macros and function prototypes used by SparingEngine code

**/

#ifndef _SPARING_ENGINE_H_
#define _SPARING_ENGINE_H_

#include <Protocol/MemRasProtocol.h>
#include <Library/mpsyncdatalib.h>
#include <Library/SynchronizationLib.h>
#include <Protocol/SmmCpuService.h>
#include <Guid/VlsVariableGuid.h>
#include <Protocol/VariableLock.h>

#include "MemRas.h"

#define MWAIT_START_QUIESCE 1
#define MWAIT_END_QUIESCE   0
#define IIO_BLOCK_QUIESCE   1
#define IIO_UNBLOCK_QUIESCE 0
#define QUIECSE_M2M         1
#define UNQUIECSE_M2M       0

typedef enum {
  RankSparing,       // Rank Sparing
  BankAdddcSparingMR,    // ADDDC Sparing
  RankAdddcSparingMR,    // ADDDC Sparing
  SddcPlusOne,
} SPARING_MODE;


EFI_STATUS
AcquireSparingEngine(
    IN UINT8 NodeId
 );


/**

  Checks the internal status and releases ownership of the sparing engine.
  It is expected that only caller who has successfully acquired ownership
  using AcquireSparingEngine calls this function

  @param NodeId - Memory controller ID

  @retval EFI_SUCCESS if sparing engine is released
  @retval EFI_UNSUPPORTED any other error

**/
EFI_STATUS
ReleaseSparingEngine(
    IN UINT8 NodeId
 );

VOID
EFIAPI
InitializeVLSInfo(
    );

EFI_STATUS
UpdateVLSVariable (
    IN BOOLEAN VLSEstablished
    );

EFI_STATUS
StartSparing(
    IN UINT8 NodeId,
    IN UINT8 SrcRank,
    IN UINT8 DstRank,
    IN UINT8 Ch,
    IN SPARING_MODE SparingMode,
    IN UINT8 RegionNum
 );

EFI_STATUS
IsCopyComplete(
    IN UINT8        Socket,
    IN UINT8
 );

VOID
M2MQuiesceUnquiesce(
    UINT8       Socket,
    UINT8       Mc,
    IN  BOOLEAN QuiesceFlag
    );

EFI_STATUS
M2MQuiescePCodeInterface(
    UINT8       Socket,
    UINT32      MailBoxCommand,
    UINT32      MailboxData
    );

VOID
SwitchAddressModeToLegacy(
   UINT8 Node
   );

VOID
SwitchAddressModeToSystem(
   UINT8 Node
   );
   
#endif // _SPARING_ENGINE_H_
