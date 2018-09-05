//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2007 - 2012, Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

    HpIOXAccess.h

Abstract:

    Header file for IOX access APIs.

-----------------------------------------------------------------------------*/
#ifndef _HPIOXACCESS_H_
#define _HPIOXACCESS_H_

#include "Platform.h"
#include <Register/PchRegsSmbus.h>
#include "RcRegs.h"
#include "SmbAccess.h"
#include <Protocol/IioUds.h>
//#include <Guid/AcpiParameter.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PciLib.h>
#include <Protocol/CpuCsrAccess.h>
#include <Library/PlatformHooksLib.h>
#include <Library/PciExpressLib.h>

//
// Produced protocols
//
#include <Protocol/HpIoxAccess.h>

#define IN_PROGRESS                EFI_ALREADY_STARTED

typedef struct {
  UINT8 Addr;
  UINT8 PortNumber;
  UINT8 Value;
}SMBUS_VALUE;

typedef
EFI_STATUS
(EFIAPI *NEXT_ENTRY_POINTER) (
  VOID
  );

typedef struct _NEXT_STATE_ENTRY {
  NEXT_ENTRY_POINTER     NextEntryPointer;
} NEXT_STATE_ENTRY;

EFI_STATUS
InitializationHPIOX(
  VOID
  );

EFI_STATUS
ReadIoxAndUpdateHpRequest(
  IN UINT8      *HpRequest,
  IN BOOLEAN    TimeSliced
  );

VOID
UpdateHpRequest(
  IN UINT8*     HpRequest
  );

void
DelayFn(
  UINT32 dCount
  );

EFI_STATUS
IssuePldCmdThroIox (
  IN UINT8           Request,
  IN UINT8           SocketId,
  IN BOOLEAN         TimeSliced
  );

EFI_STATUS
ClearAttnLatch (
  IN BOOLEAN    TimeSliced
  );

EFI_STATUS
ClearInterrupt(
  IN  BOOLEAN TimeSliced
  );

EFI_STATUS
HpSmbusReadWrite (
  IN      UINT8    SlaveAddress,
  IN      UINT8    Operation,
  IN      UINT8    Command,
  IN OUT  VOID     *Buffer,
  IN      BOOLEAN  TimeSliced
  );

BOOLEAN
CheckOnlineOfflineRequest(
VOID
  );

#endif
