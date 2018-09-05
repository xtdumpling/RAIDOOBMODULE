//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c) 2008 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  KtiRasSupport.h

Abstract:

--*/

#ifndef _CSI_RAS_SUPPORT_H
#define _CSI_RAS_SUPPORT_H

#include "KtiRas.h"

#ifdef RAS_KTI_UNIT_TEST
extern UINT16                         SmbBase;
extern UINT32                         MmCfgBase;
#endif

UINT32
ReadCpuPciCfgRas (
  PSYSHOST host,
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset
  );

VOID
WriteCpuPciCfgRas (
  PSYSHOST host,
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset,
  UINT32   Data
  );

RAS_MPLINK_STATUS
SearchQuiesceBuffer (
  IN  UINT64        Address,
  IN  UINT32        *Data32
  );

VOID
RasMpLinkClearKtiInfoBuffer (
  VOID
);

RAS_MPLINK_STATUS
RasMpLinkClearShadowCpuBufferInSteps (
  VOID
);

VOID
RasMpLinkClearBuffer (
  VOID
);

VOID
RasMpLinkClearSocketData (
  VOID
);

VOID
RasMpLinkAllocateBuffer (
  VOID
);

#ifdef RAS_KTI_MEASURE_TIME
VOID
GetExpiredTimeEntry (
  IN UINT32             MsIndex
  );
VOID
GetExpiredTimeEntryPeriod (
  VOID
  );
VOID
GetExpiredTimeExit (
  VOID
  );
#endif

#ifdef RAS_KTI_DEBUG
VOID
CsrWriteCheckPoint (
  VOID
  );

VOID
MarkEndOfCsrWrite (
  VOID
  );

VOID
DumpQuiesceBuffer (
  VOID
  );

#endif

#ifdef RAS_KTI_UNIT_TEST

EFI_STATUS
KtiRasDebugHandler (
  IN  EFI_HANDLE                    DispatchHandle,
  IN CONST EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext,
  IN OUT   VOID                         *CommBuffer,     OPTIONAL
  IN OUT   UINTN                        *CommBufferSize  OPTIONAL
  );


VOID
RegisterSwSmi (
  VOID
  );

#endif           //RAS_KTI_UNIT_TEST

#if 0
VOID
DelayFn(
  UINT32 dCount
  );

EFI_STATUS
AcquireBus (
  VOID
  );

BOOLEAN
IoDone (
  IN      UINT8           *StsReg
  );

EFI_STATUS
SmbusReadWrite (
  IN      UINT8    SlaveAddress,
  IN      UINT8    Operation,
  IN      UINT8    Command,
  IN OUT  VOID     *Buffer
  );

RAS_MPLINK_STATUS
IssuePowerOnOffCommand (
  IN UINT8          EventType,
  IN INT32          SocketType,
  IN INT32          SocketId
  );
#endif

#endif // _CSI_RAS_SUPPORT_H
